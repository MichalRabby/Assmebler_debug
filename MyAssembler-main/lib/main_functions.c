#include "../include/assembler.h"

void preAssembler(FILE *fp, char* clean_file_name) {
    char *file_name = NULL;
    char *temp_name = NULL; 
    char cur_line[MAX_LINE_LEN];
    char copy_line[MAX_LINE_LEN];
    char *token = NULL;
    FILE *newP;
    macro *temp_mac = NULL;


    /* create a new .am file with the matching file name */
    /* adding the .am ending */
    temp_name = addFileEnding(clean_file_name,1);
    file_name = (char*)malloc(strlen(temp_name)+1);/* allocate memory for file name with ending */
    check_allocation(file_name);
    strcpy(file_name,temp_name); /* file name updated with the correct .am ending */

    newP = fopen(file_name,"w");
    check_allocation(newP);

    /* going through the source file, spreading macros to the new .am file */
    while((fgets(cur_line, MAX_LINE_LEN, fp)) != NULL) {
        strcpy(copy_line,cur_line); /* copying current line */
        token = strtok(copy_line," \n\t"); /* cut first word */
        temp_mac = searchMcrList(token, head);
        if (temp_mac != NULL) {/* word found in macro list */
            copyMcrText(temp_mac,newP); /* writing macro content in the new file */
            continue; /* get next line */
        }
        if (token != NULL){
            if (strcmp(token,"mcr") == 0){
                token = strtok(NULL," \n\t"); /* cut macro name */
                if (validateMcrName(token, head)) {/* return 1 if name is new and legal. else, returns 0;  */
                    addMcr(token,fp,&head); /* add macro to list with it's content. return after endmcr */
                    continue;
                }
                else {
                    error(ERR_REDEFINITION_MACRO);
                }
                continue;
            }
        }
        if (token != NULL){
            if (strcmp(token,"mcr") == 0){
                token = strtok(NULL," \n\t"); /* cut macro name */
                if (validateMcrName(token, head)) {/* return 1 if name is new and legal. else, returns 0;  */
                    addMcr(token,fp,&head); /* add macro to list with it's content. return after endmcr */
                    continue;
                }
                else {
                    error(ERR_REDEFINITION_MACRO);
                }
                continue;
            }
        }
        /* no macro defenition or use. copy line as is */
        
        fputs(cur_line,newP);
    }
    fclose(newP);
}

/* 
*   This function scans the file and analyzes the rows according to the demands of the project.
*/
void scan_file(char *clean_file_name){
    char line[MAX_LINE_LEN];
    char *inputCopy = NULL;
    char *pointer = NULL;
    char *full_line = NULL;
    int flag;    
    FILE *fp;
    Label_Type label_type;

    inputCopy = malloc(MAX_LINE_LEN); 
    check_allocation(inputCopy);
    /* Open the .am file */
    fp = openFile(clean_file_name,1);
    if (fp == NULL){
        error(ERR_FILE_NAME);
        return;
    }

    /* Go through the file as long as there is enough RAM */
    while (((IC + DC)-2) <= RAM) {
        printf("ic + dc = %d\n", IC+DC-2);
        /* Grab the next line from the file */
        fgets(line, MAX_LINE_LEN, fp) ;
        if (line == NULL) {
            if (feof(fp)) {
                if(inputCopy != NULL)
                    free(inputCopy);
                return; 
            }
            return ;
        }
       
        fprintf(stderr,"line in code: %d\t line read: %s\n", __LINE__, line);
        /*inputCopy = (char *)malloc(strlen(line) + 1); 
        check_allocation(inputCopy);
        strcpy(inputCopy, line); */


        /* Check to see if the file is shorter than 80 characters if it isnt, go next line, else, handle. */
        if (strlen(line) == MAX_LINE_LEN - 1 && line[MAX_LINE_LEN - 2] != '\n') {                                       /* Checking to see if the array is full without \n */
            error(ERR_SIZE_LEAK);
            fprintf(stderr,"the line read is too long. %d\t cur line: %s\n", __LINE__, line);

            /* Keeps grabbing the remainder of the line so that we skip to the next one!  */
            while (strlen(line) == MAX_LINE_LEN - 1 && line[MAX_LINE_LEN - 2] != '\n'){
                /* If we reach EOF or NULL just break. */
                if (fgets(line, MAX_LINE_LEN, fp) == NULL) {
                    if (feof(fp)) {
                        free(inputCopy);
                        break; 
                    }
                }
            } 
            curr_line_number++;
        }
        
        /* line is in the right size */
        else{
            strcpy(inputCopy, line); 
            /* Grab the first word in the line */
            pointer = strtok(inputCopy, " \t\n\r\f\v");                                                               
            printf("starting switch with line: %s\n", line);
            switch (check_first_word(pointer)){
                /* Cases to ignore (; \n entry (at the moment))*/
                case 0:                                                                             
                    break;   
                /* define or externals */                                                                                           
                case 1:   
                    label_type = getLabelType(pointer);  
                    if (label_type == EXTERN_LABEL || label_type == ENTRY_LABEL){
                        pointer = strtok(NULL, " \t\n\r\f\v");
                        extern_handler(pointer,label_type); 
                    }
                    else {
                        pointer = strtok(NULL, " \t\n\r\f\v"); 
                        if (pointer == NULL){
                            error(ERR_MISSING_ARGUMENT);
                            curr_line_number++; 
                            break; 
                        }
                        define_handler(pointer,label_type);
                    }            
                    curr_line_number++;
                    break;

                /* .data .string or cmd labels */    
                case 2:
                    flag = dstring_handler(pointer); 
                    if (flag == 0){
                        curr_line_number++;
                        break;
                    }
                    else if (flag == 1){
                        curr_line_number++;
                    }
                    else if (flag == 2){
                        int len;
                        inputCopy = malloc(strlen(line) + 1); 
                        check_allocation(inputCopy);
                        strcpy(inputCopy, line);
                        len = strlen(pointer)+1;
                        full_line = inputCopy + len;\
                        check_command(full_line);
                        IC += (new_cmd -> L);
                        curr_line_number++;
                    }
                    break;

                /* Everything else */
                case 3: 
                    check_command(line);
                    if (new_cmd != NULL)
                        IC += (new_cmd -> L);
                    curr_line_number++;
                    break;
                default:
                    error(ERR_UNDEFINED_COMMAND);
                    break;
            }
        } 
        printf("end of loop!\n");
    }
    if ((IC + DC) > RAM){
        error(ERR_SIZE_LEAK);
        return;
    }
    
}


/*
*   This function makes the required files as long as err_val = 0.
*/
void makefiles(char *clean_file_name){
    char *temp_name = NULL;  
    
    if (extern_head != NULL){
        /* add the correct ending to the file */
        temp_name = addFileEnding(clean_file_name,3);
        curr_file = malloc(strlen(temp_name)+1);
        check_allocation(curr_file);
        strcpy(curr_file,temp_name);
        
        /* Creating the external file */
        make_extern();
    }
    if (entry_head != NULL){
        /* add the correct ending to the file */
        temp_name = addFileEnding(clean_file_name,4);
        curr_file = malloc(strlen(temp_name)+1);
        check_allocation(curr_file);
        strcpy(curr_file,temp_name);

        /* Creating the entry file */
        make_entry();
    }
    
    /* add the correct ending to the file */
    temp_name = addFileEnding(clean_file_name,2);
    curr_file = malloc(strlen(temp_name)+1);
    check_allocation(curr_file);
    strcpy(curr_file,temp_name);

    /* Creating the entry file */
    make_object();
    
}
