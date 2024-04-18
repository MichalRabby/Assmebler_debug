#include "../include/assembler.h"


/* this function is called only if first token of command line is NOT blank/.define/.data/.string/.extern/.entry/pottential LABEL */
/* pointer - first token of comman dline
   line - the whole command line 
   inputCopy - copy of command line, starting after command name */
void check_command(char *input) { /* input is the full command line */
    char *inputCopy = NULL ;
    int error_num = 0 ;
    int cmd_num ,i      ; 
    char *cmd_name = NULL ;
    char *token = NULL;

    /* inputCopy will hold a copy of the full input  every strtok will be cuting inputCopy */
    inputCopy = malloc(strlen(input)+1);
    check_allocation(inputCopy);
    strcpy(inputCopy, input) ;

    /* rest of line (global) will always point to the part of the line that wasnt processed yet  */
    strcpy(rest_of_line, inputCopy) ;  

    cmd_name = strtok(inputCopy,", \f\r\t\v\n") ; /* cut the first word in input */
    cmd_num = valid_command_name(cmd_name);
    if (cmd_num == -1) {
        errorCode = ERR_UNDEFINED_COMMAND ; 
        error_manager(errorCode) ;
        return ;
    }
    /* command name is valid. new_cmd node is created. cmd_num is set. L=1 */
    new_cmd = create_cmd_node(cmd_num) ; /* create a new command node with the matching command number */
    getNumOfVars() ; /* set num of required operands in totalVars */
    
    /* update rest of line pointer to after command name */    
    for (i=0; isspace(*(rest_of_line+i))!= 0 && *(rest_of_line+i) != '\n'; i++) ;
    rest_of_line = rest_of_line+i+CMD_NAME_LEN ; 
    
    /* check for illegal comma after command name  */
    for (i=0; *(rest_of_line+i) != '\n'; i++){
        if (isspace(*(rest_of_line+i)) != 0 ) /* is space */
            continue;
        else if (*(rest_of_line+i) == COMMA) {
            error_manager(ERR_ILLEGAL_COMMA);
            return ;
        }
            else
                break ;        
    }

    if (new_cmd->total_vars == FIRST_GROUP_VARS) { /* 2 operands are required */
        error_num = sourceOpCheck() ; /* send to validation. return 0 if valid. otherwise if invalid. */
        if (error_num != 0){
            errorCode = error_num ;
            error_manager(errorCode) ;
            return ;
        }
    }

    else if (new_cmd->total_vars == SECOND_GROUP_VARS){ /* 1 operand is required */
        new_cmd-> sourceAdd = 0 ; /* erelevant */
        /*new_cmd-> source1_binary = NULL ;
        new_cmd-> source2_binary = NULL ;
        free(new_cmd-> source1_binary);
        free(new_cmd-> source2_binary);*/
        error_num = targetOpCheck() ; /* send to validation */
        if (error_num != 0){
            errorCode = error_num ;
            error_manager(errorCode) ;
            return ;
        }
    }

        else if (new_cmd->total_vars == THIRD_GROUP_VARS) { /* no operands required */
            new_cmd-> sourceAdd = 0 ; /* erelevant */
            new_cmd-> targetAdd = 0 ; /* erelevant */
            /*new_cmd-> source1_binary = NULL ;
            new_cmd-> source2_binary = NULL ;
            new_cmd-> target1_binary = NULL ;
            new_cmd-> target2_binary = NULL ;
            free(new_cmd-> source1_binary);
            free(new_cmd-> source2_binary);
            free(new_cmd-> target1_binary);
            free(new_cmd-> target2_binary);*/

            token = strtok(rest_of_line, SPACE_COMMA_DEL);
            if (token != NULL ){
                errorCode = 2;
                error_manager(errorCode) ; /* Extraneous text after end of command in line */
                return ;
            }
        }

    /* if reached here, both source and target ops are valid. now we will translate the command line itself. */
    error_num = commaCheck(input) ; /* check for legal comma */
    if (error_num != 0){
            errorCode = error_num ;
            error_manager(errorCode) ;
            return ;
    }
    strcpy(new_cmd->cmd_binary,cmdBinTranslation(new_cmd -> cmd_num , new_cmd -> sourceAdd, new_cmd -> targetAdd)) ;
    
    /*printf("\nline: %d\t finishing command check with command line: %s", __LINE__,input);
    printf("\ncmdBin: %s\nsource1: %s\nsource2: %s\ntarget1: %s\ntarget2: %s\n",new_cmd->cmd_binary,new_cmd->source1_binary,new_cmd->source2_binary,new_cmd->target1_binary,new_cmd->target2_binary);
    printf("source label name: %s \t | target label name: %s\n", new_cmd->source_label, new_cmd->target_label);*/
    return ;
}

/* this function gets a word and checks if it is one of the legel commands  if yes - it creates a new command node with the mathing cmd_num. if not - error*/
int valid_command_name(char *cmd) {
    int cmd_num,i ;
    if (strlen(cmd) > CMD_NAME_LEN) {/* check valid len */
        return -1 ; /* Error: undefined command */
    }
    else {
        /* go through command name array and look for matching command */
        for (i=0; i<NUM_OF_CMDS; i++) {
            if (strcmp(cmd, commands[i])==0) { /* if command name found */
                
                cmd_num = i; /* save matching command number */
                break;
            } 
        }    
        if (i==NUM_OF_CMDS) /* command name not found */
            return -1 ; /* Error: undefined command */
    }
    
    return cmd_num ; /* no error found. i is cimmand number. */
}

/* this function fill the total_vars fIEld in the command node based on the command number.  */
void getNumOfVars() {
    int cmd_num = new_cmd->cmd_num ; 
    int vars_req ;  
    switch (cmd_num) {
        case 0:
        case 1:
        case 2:
        case 3:
            vars_req = FIRST_GROUP_VARS;
            break ;
        case 4:
        case 5:
            vars_req = SECOND_GROUP_VARS;
            break ;
        case 6:
            vars_req = FIRST_GROUP_VARS;
            break ;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            vars_req = SECOND_GROUP_VARS;
            break ;
        case 14:    
        case 15:
            vars_req = THIRD_GROUP_VARS;
            break ;
        }
    
    new_cmd -> total_vars = vars_req;
}

/* called only if command is from 1st group */
int sourceOpCheck() {
    int error_num = 0 ;
    int i;
    char *lineCopy = NULL ;
    char *op1 = NULL ;
    char *labelName = malloc(MAX_LABEL_NAME);
    size_t len ;
    int reg_num, immNum;
    int index = 9000;
    label_node *labelOp = NULL ;
    lineCopy = malloc(strlen(rest_of_line));
    check_allocation(lineCopy);
    check_allocation(labelName);
    strcpy(lineCopy, rest_of_line) ;
    op1 = strtok(lineCopy, SPACE_COMMA_DEL) ; /* cut first op */
    len = strlen(op1) ;    

    
    /* Addressing method - 0 */
    if (*op1 == '#') { /* source op is an immidiate number. */
        if (new_cmd->cmd_num == 6 ) { /* lea command - Imm addressing is illegal */
            return ERR_ILLEGAL_ADDRESSING ;
        }
        error_num = immProcessor(op1,&immNum) ; /* validation. if valid, immNum will be the imm to tranlate */

        if (error_num == 0){
            new_cmd->sourceAdd = 0 ;
            (new_cmd->L)++ ; /* increase number of bin words by 1 */
            add_ins_node(new_cmd,BinTranslation12Bit(immNum,new_cmd->sourceAdd),NULL,0,0);
            /*strcpy(new_cmd->source1_binary,BinTranslation12Bit(immNum,new_cmd->sourceAdd)) ; /* translate num to 12 bits. last 2 bit are 00 */
            /*new_cmd->source2_binary = NULL ; /* for imm source op, only 1 word required. */
            /*free(new_cmd-> source2_binary);
            /* update rest of line until after first op */
            for (i=0; (isspace(*(rest_of_line+i))!= 0 || (*(rest_of_line+i) == COMMA)) && *(rest_of_line+i) != '\n' ; i++);
            rest_of_line = rest_of_line+i+len ; 
            return targetOpCheck() ;
        }
        else
            return error_num ; /* operand in undefined. */

    }
    /* Addressing method - 3 */
    reg_num = isReg(op1) ;
    if (reg_num != -1 ) { /* source op is a register. */
        if (new_cmd->cmd_num == 6 )  /* lea command - Reg addressing is illegal */
            return ERR_ILLEGAL_ADDRESSING ;
        new_cmd->sourceAdd = 3 ;     
        (new_cmd->L)++ ; /* increase number of bin words by 1 */
        add_ins_node(new_cmd,RSBinTranslation(reg_num),NULL,0,3);
        /*strcpy(new_cmd->source1_binary, RSBinTranslation(reg_num)) ;
        /* update rest of line until after first op */
        /* new_cmd->source2_binary = NULL; /* for register source op, only 1 word required. */\
        /* free(new_cmd-> source2_binary);*/
        for (i=0; (isspace(*(rest_of_line+i))!= 0 || (*(rest_of_line+i) == COMMA)) && *(rest_of_line+i) != '\n'; i++);
        rest_of_line = rest_of_line+i+len ; 
        return targetOpCheck() ;
        /* source operand is a legal register. RSbin word was already translated. */
    }
    

    /* Addressing method - 1 */
    if ((labelOp = label_exists(op1)) != NULL) { /* source op is a label. */
        /* strcpy(new_cmd-> source_label, labelOp-> label_name) ; /* save label name for later translation */
        new_cmd->sourceAdd = 1; /* set addressing method */
        (new_cmd->L)++ ; /* increase number of bin words by 1 */
        add_ins_node(new_cmd,"??????????????",labelOp->label_name,0,1);
        /*strcpy(new_cmd->source1_binary,"??????????????") ; /* mark for later translation */
        /*new_cmd->source2_binary = NULL ; /* for label source op, only 1 word required. */
        /*free(new_cmd-> source2_binary);
            /* update rest of line until after first op */
            for (i=0; (isspace(*(rest_of_line+i))!= 0 || (*(rest_of_line+i) == COMMA)) && *(rest_of_line+i) != '\n'; i++){}
            rest_of_line = rest_of_line+i+len ; 
            return targetOpCheck() ;
    }

    /* return 0 if source op is an array_index. */
    /* return 1 for error. return -1 for unrecognized label, but valid index. */
    /* Addressing method - 2 */
    error_num = isIndex(&index, &labelOp, &labelName)  ; /* if valid, rest of line is updated */
    if (index != 9000){
        if (index < 0){
            return ERR_SEGMENTATION_FAULT;
        }
        /*new_cmd -> source_index = index;*/
    }
    /*strcpy(new_cmd-> source_label, labelName);*/

    if (error_num == 1)  /* error found */
        return ERR_UNDEFINED_ARGUMENT; 
    else if (error_num == -3)
        return ERR_SEGMENTATION_FAULT ;    
    else if (error_num == 0) { 
        new_cmd->sourceAdd = 2; /* set addressing method */
        (new_cmd->L)++ ;/* increase number of bin words by 1 */
        add_ins_node(new_cmd,"??????????????",labelName,index,2);
        (new_cmd->L)++ ;/* increase number of bin words by 1 */
        add_ins_node(new_cmd,BinTranslation12Bit(index,0),labelName,index,2);
        /*strcpy(new_cmd->source1_binary,"??????????????") ; /* mark later translation of label address */
        /*strcpy(new_cmd->source2_binary, BinTranslation12Bit(index,0)) ; /*  tranlate the index value. */
        return targetOpCheck(); 
    }
    else if (error_num == -1) { /* not an index method  still possible it is a label undefined yet. */
            for (i=0; (isspace(*(rest_of_line+i))!= 0 || (*(rest_of_line+i) == COMMA)) && *(rest_of_line+i) != '\n' ; i++);
            rest_of_line = rest_of_line+i+len ; 
            new_cmd-> sourceAdd = 1 ; 
            /*strcpy(new_cmd-> source_label, op1); /* not index method. op2 supposed to be the whole label name */
            /*strcpy(new_cmd->source1_binary,"??????????????") ; /* optional label  */
            /*new_cmd->source2_binary = NULL ; /* for addressing method 1 - only 1 word required.  \0 */
            /*free(new_cmd-> source2_binary);*/
            (new_cmd->L)++ ; /* increase word count */
            add_ins_node(new_cmd,"??????????????",op1,0,1);
            return targetOpCheck();
    }
    else {/* error_num == -2. index found  label not  */
        new_cmd->sourceAdd = 2; /* set addressing method */
        (new_cmd->L)++ ; /* increase number of bin words by 2 */
        /*strcpy(new_cmd->source1_binary,"??????????????") ; /* mark for optional label defining later */
        /*strcpy(new_cmd->source2_binary, BinTranslation12Bit(index,0)) ; /* tranlate the index value */ 
        add_ins_node(new_cmd,"??????????????",op1,index,2);
        (new_cmd->L)++ ; /* increase number of bin words by 1 */
        add_ins_node(new_cmd,BinTranslation12Bit(index,0),op1,index,2);
        return targetOpCheck();
    }
    return ERR_UNDEFINED_ARGUMENT ; /* if source op doesnt fit the prior options, it is undefined. */
    
}

/* return 0 if source op is a valid array_index. 
    return 1 for error. 
    return -1 if no ] found, still possible that op1 is a label not defined yet. 
    return -2 for unrecognized label, but valid index. 
    return -3 if there is a seg fault */
int isIndex(int *index, label_node **baseLabel, char **labelName) {
    int i, j, k, s,temp;
    char baseLabelName[32] ; /* assuming label name is no longer than 32 chars*/
    char indexS[32] ;
    label_node *labelIndex = NULL ;
    size_t len = strlen(rest_of_line) ;

    /* get index */
    for (i=0; (*(rest_of_line+i) != ']') && i <= len ; i++) ;
    if (i==len+1 && *(rest_of_line+i) != ']') /*  reach end of input */
        return -1; /* no ] found. not addressing method 2. still possible that it's a label not defined yet. */
    /* ] found. look for [ opener */
    for (j=i; *(rest_of_line+j)!='[' && j >= 0 ; j--) ;
    if (j==0) /* [ opener not found */
        return 1 ; /* error - ] found but not [ */
    /* j points to the [, i points to the ]. copy the middle without spaces to indexS[] */    
    for (k=j+1, s=0 ; k<i; k++) {
        if (isspace(*(rest_of_line+k)) != 0)
            continue ;
        else { /* copy */
            indexS[s] = *(rest_of_line+k) ;
            s++ ;
        }
    }
    indexS[s] = '\0'; /* string null terminator */
    /* indexS is now the index. check if it is a label or a number */
    if (isalpha(indexS[0]) != 0) {/* a letter. check label list */
        labelIndex = label_exists(indexS) ;
        if (labelIndex == NULL) /* label not found */
            return 1 ;
        if (labelIndex->label_type != DEF_LABEL)
            return 1 ; /* only .define labels can be used here */    
        /* index label found. get value */
        *index = labelIndex->definedData ; 

    }
    else { /* a number or symbol. check if integer. */
        if (isNumber(indexS,&temp) != 0 ) /* not a number */
            return 1; /* index is not a label and not a number. invalid */
    
        else
            *index = temp ;
        }
    /* if reached here - index is valid. continue to check label validation. */
    /* copy string up to j to the labelBaseName[] */
    for (k=0, s=0 ; k<j; k++) {
        if ((isspace(*(rest_of_line+k))) != 0 || *(rest_of_line+k) == COMMA)
            continue ;
        baseLabelName[s] = *(rest_of_line+k) ;
        s++ ;
    }
    baseLabelName[s] = '\0';
    strcpy(*labelName,baseLabelName);
    *baseLabel = label_exists(baseLabelName) ;
    /* checking that index is not out of the array bounderies */
    if (*baseLabel != NULL) { /* valid array[index] addressing method. the base address will be calulated later. */
        if (((*baseLabel) -> label_type == DATA_LABEL || (*baseLabel) -> label_type == STRING_LABEL)){
            if ((*baseLabel)-> data_count <= *index || *index <= 0){
                return -3 ; 
            }
            rest_of_line = rest_of_line+i+1;
            return 0;
        }
        rest_of_line = rest_of_line+i+1; /* update rest of line to point after ] */
        return 0; 
    }
    else {
        rest_of_line = rest_of_line+i+1; /* update rest of line to point after ] */
        return -2 ; /* optional that the label will be fefined later. */
    }
    

    
    
}


int targetOpCheck() {    
    int reg_num, immNum , index ;
    int error_num = 0 ;
    char *lineCopy  = NULL ;
    char *op2 = NULL ;
    char *extra = NULL ;
    char *temp = NULL ;
    char *labelName = malloc(MAX_LABEL_NAME);
    label_node *labelOp = NULL ;
    lineCopy = malloc(strlen(rest_of_line));
    check_allocation(lineCopy);
    check_allocation(labelName);
    temp = malloc(BIN_WORD_LEN+1) ;
    check_allocation(temp);
    strcpy(lineCopy, rest_of_line) ;
    op2 = strtok(lineCopy, SPACE_COMMA_DEL) ; /* cut 2 opFimm */


    /* Addressing method - 0 */
    if (*op2 == '#') { /* source op is an immidiate number. */
        if ((new_cmd->cmd_num != 1) && (new_cmd->cmd_num != 12)) { /*  Imm addressing is legal only for cmp/prn commands.  */
            return ERR_ILLEGAL_ADDRESSING ;
        }
        error_num = immProcessor(op2,&immNum) ; /* validation. if valid, immNum will be the imm to tranlate */
        if (error_num != 0)
            return error_num ; /* operand in undefined. */
        else {
            new_cmd->targetAdd = 0 ;
            (new_cmd->L)++ ; /* increase number of bin words by 1 */
            /*strcpy(new_cmd->target1_binary,BinTranslation12Bit(immNum,new_cmd->targetAdd)) ; /* translate num to 12 bits. last 2 bit are 00 */
            /*new_cmd->target2_binary = NULL ; /* for imm source op, only 1 word required. */
            /*free(new_cmd-> target2_binary);
            /* source operand is a legal immediate. bin word was already translated. */
            /* update rest of line until after op2 */
            add_ins_node(new_cmd,BinTranslation12Bit(immNum,new_cmd->targetAdd),NULL,0,0);
            extra = strtok(rest_of_line, SPACE_COMMA_DEL);
            extra = strtok (NULL, op2);
            return checkExtra(extra) ;         
        }
    }
    /* Addressing method - 3 */
    /* if source op was also a register. we need to combine the bin words. */
    else if ((reg_num = isReg(op2)) != -1 ) { /* source op is a register. */
            new_cmd->targetAdd = 3 ; 
            if (new_cmd->sourceAdd != 3) {  
                (new_cmd->L)++ ; /* increase number of bin words by 1 */
                /*strcpy(new_cmd->target1_binary, RTBinTranslation(reg_num)) ;
                new_cmd->target2_binary =  NULL ; /* for register source op, only 1 word required. */
                /* source operand is a legal register. RSbin word was already translated. */
                add_ins_node(new_cmd,RTBinTranslation(reg_num),NULL,0,3);
                extra = strtok(rest_of_line, SPACE_COMMA_DEL);
                extra = strtok (NULL, op2);
                return checkExtra(extra) ;            
            }
            else { /* both source and target ops are registers */
                /*strcpy(new_cmd->target1_binary, RTBinTranslation(reg_num)) ;*/
                temp = combineRegBin(new_cmd->next_ins->bin,RTBinTranslation(reg_num)) ;
                strcpy(new_cmd->next_ins->bin,temp) ;
                /*new_cmd->target2_binary = NULL ; /* for register source op, only 1 word required. */
                /*new_cmd->target1_binary = NULL ;
                /*free(new_cmd-> target1_binary);
                /*free(new_cmd-> target2_binary);*/
                /* source operand is a legal register. RSbin word was already translated. */
            }
            /* target reg is valid. check extra text */
            extra = strtok(rest_of_line, SPACE_COMMA_DEL);
            extra = strtok (NULL, op2);
            return checkExtra(extra) ;         
        }


        /* Addressing method - 1 */
        else if ((labelOp = label_exists(op2)) != NULL) { /* source op is a label. */
            /*strcpy(new_cmd-> target_label, labelOp-> label_name) ; /* save label name for later translation */
            new_cmd->targetAdd = 1; /* set addressing method */
            (new_cmd->L)++ ; /* increase number of bin words by 1 */
            add_ins_node(new_cmd,"??????????????",labelOp->label_name,0,1);
            /*strcpy(new_cmd->target1_binary,"??????????????") ; /* translate num to 12 bits. last 2 bit are 00 */
            /*new_cmd->target2_binary = NULL ; /* for label source op, only 1 word required. */
            /*free(new_cmd-> target2_binary);
            /* source operand is a legal label. bin word was already translated. */
            extra = strtok(rest_of_line, SPACE_COMMA_DEL);
            extra = strtok (NULL, op2);
            return checkExtra(extra) ;         
            
        }
        else {
        /* return 0 if source op is an array_index. */
        /* return 1 for error. return -1 for unrecognized label, but valid index. */
        /* Addressing method - 2 */
        
        error_num = isIndex(&index, &labelOp, &labelName)  ; /* if valid, rest of line is up to date */
        if (index != 9000){
            if (index < 0){
                return ERR_SEGMENTATION_FAULT;
            }
            /*new_cmd -> target_index = index;*/
        }

        /* rest_of_line has moved to after op2 */
        /*strcpy(new_cmd-> target_label, labelName);*/
        if (error_num == 1)  /* error found */
            return ERR_UNDEFINED_ARGUMENT;
        else if (error_num == -3){
            return ERR_SEGMENTATION_FAULT ;
            }
            else if (error_num == 0) { 
                if ((new_cmd->cmd_num == 9) || (new_cmd->cmd_num == 10) || (new_cmd->cmd_num == 13))  /*  index addressing is illegal for jmp/bne/jsr commands.  */
                    return ERR_ILLEGAL_ADDRESSING ;
                new_cmd->targetAdd = 2; /* set addressing method */
                (new_cmd->L)++ ; /* increase number of bin words by 2 */
                /*strcpy(new_cmd->target1_binary,"??????????????") ; /* mark later translation of label address */
                /*strcpy(new_cmd->target2_binary, BinTranslation12Bit(index,0)) ; /*  tranlate the index value. */
                add_ins_node(new_cmd,"??????????????",labelName,index,2);
                (new_cmd->L)++ ; /* increase number of bin words by 2 */
                add_ins_node(new_cmd,BinTranslation12Bit(index,0),labelName,index,2);
                /* source operand is a legal label[index]. */
                }
                else if (error_num == -1) { /* not an index method  still possible it is a label undefined yet. */
                    new_cmd-> targetAdd = 1 ; 
                    /*strcpy(new_cmd-> target_label, op2); /* not index method. op2 supposed to be the whole label name */
                    /*strcpy(new_cmd->target1_binary,"??????????????") ; /* optional label  */
                    /*new_cmd->target2_binary = NULL ; /* for addressing method 1 - only 1 word required.  NULL */
                    /*free(new_cmd-> target2_binary);*/
                    (new_cmd->L)++ ; /* increase word count */
                    add_ins_node(new_cmd,"??????????????",op2,0,1);
                    /* optional label will be defined later */
                    extra = strtok(rest_of_line, SPACE_COMMA_DEL);
                    extra = strtok (NULL, op2);
                    return checkExtra(extra);
                }
                    else {/* error_num == -2. index found  label not  */
                        if ((new_cmd->cmd_num == 9) || (new_cmd->cmd_num == 10) || (new_cmd->cmd_num == 13)) { /*  index addressing is illegal for jmp/bne/jsr commands.  */
                                return ERR_ILLEGAL_ADDRESSING ;
                    }
                        new_cmd->targetAdd = 2; /* set addressing method */
                        (new_cmd->L)++ ; /* increase number of bin words by 2 */
                        /*strcpy(new_cmd->target1_binary,"??????????????") ; /* mark for optional label defining later */
                        /*strcpy(new_cmd->target2_binary, BinTranslation12Bit(index,0)) ; /* tranlate the index value */ 
                        add_ins_node(new_cmd,"??????????????",op2,index,2);
                        (new_cmd->L)++ ; /* increase number of bin words by 2 */
                        add_ins_node(new_cmd,BinTranslation12Bit(index,0),op2,index,2);
                        /* index is valid. label doesnt exists. optioanl later defenition of label */
                    }
    }
    return checkExtra(rest_of_line) ;

}

/* this function is called if the source op statrs with a '#'. validates the imm value. return 0 for valid. else error num */
int immProcessor(char *token, int *immNum) {
    char *immP = token+1 ; /* sets on the char after '#' */
    label_node *lblP = NULL ;
    if (immP == NULL) /* blank */
        return ERR_MISSING_ARGUMENT ; /* missing argumant */
    if ((lblP = label_exists(immP)) != NULL ) { /* imm is a label */
        if (lblP->label_type != DEF_LABEL)
            return ERR_UNDEFINED_ARGUMENT ; 
        /* label found, type is define. */
        *immNum = lblP->definedData ;

    }
    else if (isNumber(immP, immNum) == 1 ) /* imm is a valid number */
        return ERR_UNDEFINED_ARGUMENT ; 
    /* valis number saved in immNum. check range */
    if (rangeCheck(*immNum) == 1)
        return ERR_IMM_OVERFLOW ; /* can't be represented in 12 bits. */

    /* imm is legal. the value is saved in num. continue to bin-tranlation */
    return 0 ;
}

/* this function gets a string and a pointer to int  if the string is a number, it saves it in *num and returns 0. else, return 1*/
int isNumber(char *imm, int *num) {
    char *end_P = NULL ;
    double temp_num = strtod(imm,&end_P); /* convert string to number */
    if (strcmp(end_P,"")!=0) /* chars that are not numbers */
        return 1 ; /* not a real number */
    /* real number. check if integer */ 
    if ((int)(temp_num) == temp_num ) {
        *num = temp_num ;

        return 0 ;
    }
    else
        return 1; /* a number but not an integer */
}

/* this functions gets a number and translate it to 12 bits bunary representation. the ARE bits will be set to 00 */
char *BinTranslation12Bit(int num, int ARE) {
    static char result[BIN_WORD_LEN];
    int i, bit, index = 0 ;
    int negHandle = 1 << BITS_IN_INT ; /* for neg numbers, add 4096 to get the correct Two's complement bin representation */
    /* Handle negative numbers using Two's complement */
    if (num < 0) {
        num += negHandle ;
    }
    /* Store the binary representation of num in the string */
    for (i = BITS_IN_INT - 1; i >= 0; i--) {
        bit = (num >> i) & 1;
        result[index] = bit + '0'; 
        index++ ;
    }
    switch (ARE) {
        case 0:
            /* set the ARE fIEld to 00 */
            result[LSB-1] = '0' ;
            result[LSB] = '0' ;
            result[LSB+1] = '\0'; 
            break ;
        case 1:
            /* set the ARE fIEld to 01 */
            result[LSB-1] = '0' ;
            result[LSB] = '1' ;
            result[LSB+1] = '\0'; 
            break ;
        case 2:
            /* set the ARE fIEld to 01 */
            result[LSB-1] = '1' ;
            result[LSB] = '0' ;
            result[LSB+1] = '\0'; 
            break ;

    }
    return result ;
}

/* this function gets a string and checks if it matches one of the register names. return reg number if found  otherwose return -1. */
int isReg(char *token) {
    int i = 0 ;
    for (; i<(NUM_OF_REGS-2); i++) { /* last 2 registers are not allowed to be used on commands */
        if (strcmp(token, registers[i]) == 0 ) /* register found */
            return i ;    
        
        else
            continue;
    }
    return -1 ;
}

/* this function gets a reg number. translate to binary and sets the number in the RS bit field. */
char *RSBinTranslation(int reg_num) {
    static char result[BIN_WORD_LEN] ;
    int num = reg_num << RS_SHIFT ; /* shifting left to get the RS number in the 5-7 bits */
    strcpy(result, BinTranslation12Bit(num,0)) ; /* use imm translation func */  
    return result ;
}

/* this function gets a reg number. translate to binary and sets the number in the RT bit field. */
char *RTBinTranslation(int reg_num) {
   static char result[BIN_WORD_LEN] ;
    strcpy(result, BinTranslation12Bit(reg_num,0)) ; /* use imm translation func */  
    return result ;
}

/* this function cmd number, source and target addressing mthods, and translate it to the first cnd binary word. */
char *cmdBinTranslation(int cmd_num, int sourceAdd, int targetAdd) {
    static char result[BIN_WORD_LEN+1] ;
    int i,j;
    char opcode[OPCODE_BIN_LEN] ;
    /* left 4 bits are always set to 0 */
    for (i=0; i<=3; i++) {
        result[i] = '0' ;
    }

    /* set opcode */
    strcpy(opcode,opcodeBinTranslation(cmd_num)) ;

    for (i=OPCODE_FIELD, j=0; i<=OPCODE_FIELD+OPCODE_BIN_LEN; i++, j++) {
        result[i] = opcode[j] ;
    }
    
    /* set source op addressing method in bits [5-4]*/
    switch (sourceAdd) {
        case 0:
            result[RSA_FIELD] = '0';
            result[RSA_FIELD+1] = '0';
            break ;
        case 1:
            result[RSA_FIELD] = '0';
            result[RSA_FIELD+1] = '1';
            break ;
        case 2:
            result[RSA_FIELD] = '1';
            result[RSA_FIELD+1] = '0';
            break ;
        case 3:
            result[RSA_FIELD] = '1';
            result[RSA_FIELD+1] = '1';
            break ;
    }
    /* set target op addressing method in bits [3-2]*/
    switch (targetAdd) {
        case 0:
            result[RTA_FIELD] = '0';
            result[RTA_FIELD+1] = '0';
            break ;
        case 1:
            result[RTA_FIELD] = '0';
            result[RTA_FIELD+1] = '1';
            break ;
        case 2:
            result[RTA_FIELD] = '1';
            result[RTA_FIELD+1] = '0';
            break ;
        case 3:
            result[RTA_FIELD] = '1';
            result[RTA_FIELD+1] = '1';
            break ;
    }
    /* right 2 bits are always set to 0 */
    result[BIN_WORD_LEN-1] = '0';
    result[BIN_WORD_LEN - 2] = '0' ;
    result[BIN_WORD_LEN] = '\0'; 
    return result; 
}

/* this function gets a number. if it can be represented in 12 bits - return 0. otherwise, return 1.  */
int rangeCheck(int num) {
    if (num <= MIN_12BITS || num >= MAX_12BITS ) {
       return 1 ; /* number can't be represented in 12 bits. */
    }
    return 0 ; /* valid immidiate  immNum is the cur number. */
}

/* this function gets a number and translate it to a 4 bit string */
char *opcodeBinTranslation(int num) {
    static char result[OPCODE_BIN_LEN +1]; 
    int bit, i;
    int index = 0;
    
    for (i = OPCODE_BIN_LEN-1; i >= 0; i--) { 
        bit = (num >> i) & 1;
        result[index] = bit + '0'; 
        index++;
    }
    result[index] = '\0'; 
    return result; 
}

/* this function gets 2 binary string and returns the OR bitwise string. */
char *combineRegBin(char *str1, char *str2) {
    int i;
    static char result[BIN_WORD_LEN+1] ;
    for (i=0; i<=BIN_WORD_LEN; i++) {
        if (*(str1+i) == '1' || *(str2+i) == '1')
            result[i] = '1' ;
        else
            result[i] = '0' ;
    }
    result[BIN_WORD_LEN] = '\0' ;
    return result ;

}

/* this function gets the cmd node and the input line copy. checks fer legal commas. returns 0 if valid. otherwose returns error number. */
int commaCheck(char *input_copy) {
    int comma_count = 0 ; /* comma appereance counter */
    int consecutive_comma = 0 ; /* consecutive comma counter */
    char *input = input_copy + CMD_NAME_LEN ; /* jump over command name */
    /*int cmd_num = my_data->cmd_num ;*/
    int len = strlen(input) ;
    int i , comma_req ; /* num of commas required for each command type */
    if (*input==COMMA)
        return 6; /* Error: illegal comma after command */
    for (i=0; i<len; i++){
        if (*(input+i)==COMMA){ /* if current char is a comma, count.  */
            comma_count++;
            consecutive_comma++;
            continue ;
        }
        if (isspace(*(input+i))!=0) /* if current char is a space, move on. */
            continue ;
        else  /* current char is not a space and not a comma */
            consecutive_comma = 0 ; /* zero consecutive counter */   
    }
    if (consecutive_comma > 1)
        return 8 ; /* Error: multiple consecutiva commas error */
    /* set required number of commas */
    if (new_cmd->total_vars == SECOND_GROUP_VARS || new_cmd->total_vars == THIRD_GROUP_VARS) 
        comma_req = 0 ;
    else if (new_cmd->total_vars == FIRST_GROUP_VARS) 
        comma_req = 1 ;

    if (comma_count < comma_req)
        return 7 ; /* Error: missing comma */
    if (comma_count > comma_req)
        return 6 ; /* Error: illegal comma */
    else
        return 0 ; /* no error */
    
}

int checkExtra(char *extra) {
    char *temp = strtok(extra," \f\r\t\v\n");
        if (temp != NULL)
            return ERR_EXTRANEOUS_TEXT ;
        else
            return 0; 
}
    




