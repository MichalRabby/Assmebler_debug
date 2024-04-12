#include "../include/assembler.h"

/*
* Returns 0 if error, 1 if ok, 2 if needs cmd check
*/
int dstring_handler(char *pointer){
    char *label_name = NULL;                                            /* Copies the pointer */
    char *p_copy = NULL;
    char *suggested_cmd = NULL;
    int i = 0;
    Label_Type label_type;
    label_node *temp_node = NULL;
    label_name = pointer;

    /* Replace the ':' with \0 */
    while (pointer[i] != ':'){
        i++;
    }
    pointer[i] = '\0';

    /* Save the label_name and increment pointer */
    label_name = malloc(strlen(pointer)+1);
    check_allocation(label_name);
    strcpy(label_name,pointer);

    pointer = strtok(NULL," \t\n\r\f\v");   
    suggested_cmd = pointer;                      
    
    /* Check to see if missing argument */
    if (pointer == NULL ){
        error(ERR_MISSING_ARGUMENT);
        return 0;
    }
    
    else{
        label_type = getLabelType(pointer);
        /* If LABEL: .entry or LABEL: .extern we ignore */
        if (strcmp(pointer,".entry") == 0 || strcmp(pointer,".extern") == 0){
            return 0;
        }

        /* Check the label */
        if (!check_label(label_name,label_type)){
            return 0;
        }


        /* Check the remainding text */
        pointer = strtok(NULL,"\f\r\t\v\n");
        if (pointer == NULL && strcmp(suggested_cmd,"hlt") != 0 && strcmp(suggested_cmd,"rts")!= 0){
            error(ERR_MISSING_ARGUMENT);
            return 0;
        }
        
        /* Fetch data according to label type */
        if (label_type == STRING_LABEL || label_type == DATA_LABEL){
            p_copy = malloc(strlen(pointer)+1);
            check_allocation(p_copy);
            strcpy(p_copy,pointer);

            /* CHECK THE DATA */
            if (!check_data(p_copy,label_type)){
                return 0;
            }
            /* If a label doesnt exist, add a new one */
            temp_node = label_exists(label_name);
            if (temp_node != NULL)
                add_label(DC,DC,label_name,1,label_type);
            else
                add_label(DC,DC,label_name,0,label_type);
            

            /* search & grab the new node */
            temp_node = dc_head;
            while (temp_node != NULL && !((temp_node -> label_type) == label_type && strcmp((temp_node -> label_name),label_name) == 0)){
                temp_node = temp_node -> next_label;
            }
           
            /* Fetch data according to label type */
            if (label_type == STRING_LABEL){
                fetch_data(p_copy,temp_node);
                DC = DC + (temp_node -> data_count);
                return 1;                
            }
            else if (label_type == DATA_LABEL){
                fetch_data(p_copy,temp_node);
                DC = DC + (temp_node -> data_count);
                return 1;
            }

        }
        else if (label_type == CMD_LABEL){
            if (temp_node != NULL)
                add_label(IC,0,label_name,1,label_type);
            else 
                add_label(IC,0,label_name,0,label_type);
            return 2;
        }
        else if (label_type == INVALID){
            error(ERR_UNDEFINED_COMMAND);
            return 0;
        }
    }
    return 1;
}

int check_data(char *pointer,Label_Type label_type){
    /* Copy the string for safety */
    int i,counter,len;
    char *string = NULL;
    label_node *temp = NULL;
    counter = 0;
    string = malloc(strlen(pointer)+1);
    check_allocation(string);
    strcpy(string,pointer);
    
    /* Check the string according to label type */
    if (label_type == STRING_LABEL){
        
        /* As long as last char is useless, decrease the string */
        while (string[strlen(string)-1] == '\n' || string[strlen(string)-1] == '\t' || string[strlen(string)-1] == ' '){
            string[strlen(string)-1] ='\0';
        }

        /* If string is missing parentheses  */
        if (string[0] != '"' && string[strlen(string)-1] != '"'){
            error(ERR_MISSING_PARENTHESES);
            return 0;
        }

        /* If string is: "  */
        else if ((string[0] == '"' && string[strlen(string)-1] != '"')){
            len = strlen(string);
            for (i = 0; i < len; i++){
                if (string[i] == '"'){
                    counter++;
                }
            }
            if (counter >= 2 ){
                error(ERR_EXTRANEOUS_TEXT);
                return 0;
            }
            if (counter < 2){
                error(ERR_MISSING_PARENTHESES);
                return 0;
            }
        }

        /* If string is:    " */
        else if ((string[0] != '"' && string[strlen(string)-1] == '"')){
            len = strlen(string);
            for (i = 0; i < len; i++){
                if (string[i] == '"'){
                    counter++;
                }
                if (counter >= 2 || counter < 2){
                    error(ERR_MISSING_PARENTHESES);
                    return 0;
                }
            }
        }
    }
    else if (label_type == DATA_LABEL){
        size_t i = 0, size_remainder;
        int numCounter = 0, total_comma = 0, total_comma_in_a_row = 0; 
        char *remainder_copy;
        char *pp = NULL;
        remainder_copy = (char*)malloc(strlen(pointer) + 1);   
        check_allocation(remainder_copy);
        strcpy(remainder_copy,pointer);                                               /* Copies the input */ 
        size_remainder = strlen(remainder_copy); 
        pp = strtok(string," ,\t\n\r\f\v");

        /* As long as we have not reached the end of the input */
        while (pp != NULL ){     
            
            /* If the current pointer string is all alphabetical letters, it must be a define! */                    
            if (check_alpha(pp)) {
                temp = label_exists(pp);
                /* If no such define exists, or if a label exists but its NOT a define! */
                if (temp == NULL || ((temp -> label_type) != DEF_LABEL) ){
                    error(ERR_UNDEFINED_ARGUMENT);
                    return 0;
                }
            }
            /* If its not fully alphabetical, than it must be a number! RIGHT? So lets check the value! if its OOVVEERR 9000! (meme) its no good... */
            else if (strToInt(pp) == 9000){
                error(ERR_UNDEFINED_ARGUMENT);
                return 0;
            }
            numCounter++;
            pp = strtok(NULL," ,\t\n\r\f\v");
        } 
        
        /* The maximum amount of numbers is 36 (with 1 letter label & 35 commas) */
        if (pp != NULL && numCounter >= 36){                                                   
            error(ERR_EXTRANEOUS_TEXT);
            return 0;
        }
        
        /* Checks for extra and consecutive commas */
        for (i = 0; i < size_remainder; i++){            
            if (!WHITESPACE(remainder_copy[i])){
                if (remainder_copy[i] == ','){
                    if (i == 0){
                        error(ERR_ILLEGAL_COMMA);
                    return 0;
                    }
                    total_comma++;
                    total_comma_in_a_row++;
                    if (total_comma_in_a_row >= 2){
                        error(ERR_MULTIPLE_CONSECUTIVE_COMMAS);
                        return 0;
                    }
                }
                else {
                    total_comma_in_a_row = 0;
                } 
            }
        }
        /* These statements return errors according to what we previously found out */
        if ((numCounter == 0 && total_comma >= 1) || total_comma > 35 ){
            error(ERR_ILLEGAL_COMMA);
            return 0;
        }
        else if (total_comma < numCounter-1){
            error(ERR_MISSING_COMMA);
            return 0; 
        }
        else if (total_comma >= numCounter){
            error(ERR_EXTRANEOUS_TEXT);
            return 0;
        }
    }
    free(string);
    return 1;
}

void fetch_data(char *p_copy, label_node *temp_node){

    /* Copy the string for safety */
    int i,len;
    char *string = NULL;
    char *pointer = NULL;
    char *num;
    label_node *ptr;
    string = malloc(strlen(p_copy)+1);
    check_allocation(string);
    strcpy(string,p_copy);
    /* We now fetch according to the label type */
    /* For string, just go through the string and translate to ASCII */
    if (temp_node -> label_type == STRING_LABEL){
        while (string[strlen(string)-1] == '\n' || string[strlen(string)-1] == '\t' || string[strlen(string)-1] == ' '){
            string[strlen(string)-1] ='\0';
        }
        len = strlen(string)-2;
        string = string+1;
        for (i = 0; i < len; i++){
            add_data(string[i],temp_node);
        }
        add_data(0,temp_node);
    }

    /* If its a data label */
    else if (temp_node -> label_type == DATA_LABEL){
        pointer = strtok(string," ,\t\n\r\f\v");
        
        /* Go through the data array that we received */
        while (pointer != NULL){
            num = malloc(strlen(pointer)+1);
            check_allocation(num);
            strcpy(num,pointer);

            /* If we find a pointer that is not a number .. it must be a define label, since we already checked !!! */
            if (strToInt(num) == 9000){
                ptr = label_exists(num);

                /* Add the defined Data */
                if (ptr != NULL){
                    add_data(ptr -> definedData,temp_node);
                }
            }
            else{
                add_data(strToInt(num),temp_node);
            }
            pointer = strtok(NULL," ,\t\n\r\f\v");
        }
    }
}
