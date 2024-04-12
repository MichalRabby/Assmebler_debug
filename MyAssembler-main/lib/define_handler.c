#include "../include/assembler.h"


/* 
*   This function handles all the different cases of .define
*/
void define_handler(char *pointer, Label_Type label_type){
    char *p_copy = pointer;                                                             /* Copies the pointer */
    p_copy = strtok(NULL,"\t\n\r\f\v");                                                 /* Increments the copy (pointer stays the same as before in father func) */
    /* If there is no remainder, gets sent to case 1*/
    if (p_copy == NULL){
        def_case_1(pointer, label_type);
        return;
    }
    else if (p_copy != NULL){
        if (p_copy[0] == '='){
            if (strlen(p_copy) == 1){
                p_copy = strtok(NULL," \t\n\r\f\v");    
                def_case_4(pointer,label_type,p_copy);                                  /* If = is at the begginning of the variable, case 4*/
            }
            else {
                p_copy[0] = ' ';
                def_case_2(pointer,label_type,p_copy);                                  /* If = is not connected to the label OR the variable, case 2*/
            }
        }
        else if (pointer[strlen(pointer)-1] == '='){                        
            pointer[strlen(pointer)-1] = '\0';
            def_case_3(pointer,label_type,p_copy);                                      /* If = is at the end of the label, case 3*/
        }
        else if (!check_label(pointer,label_type))
            return;
    }
}

/*
*   Handles the first case of define data input. case 1:  .define label=num
*/
void def_case_1(char *pointer, Label_Type label_type){
    int len,i;
    char *label_name = NULL;
    char *num;
    char *p_copy;                                     /* Copies the pointer */
    label_node *temp = NULL;
    p_copy = pointer;
    len = strlen(p_copy);
    /* Find '=' inside of the string */
    for (i = 0; i < len; i++){
        if (p_copy[i] == '='){
            p_copy[i] = ' ';
            len = -1;
            break;
        }
    }

    /* If there is no '=' return error */
    if (len != -1){
        error(ERR_UNDEFINED_ARGUMENT);
        return;
    }

    /* Check the label */
    p_copy = strtok(p_copy," \t\n\r\f\v");
    
    if (!check_label(p_copy,label_type)){
        return;    
    }

    /* Copy label name to variable */
    label_name = malloc(strlen(p_copy));
    check_allocation(label_name);
    strcpy(label_name,p_copy);

    /* Grab the duplicate (if exists) */
    temp = label_exists(label_name);

    /* Advance to define data & store in num variable to check with atoi */
    p_copy = strtok(NULL," \t\n\r\f\v");
    if (p_copy == NULL){
        error(ERR_MISSING_ARGUMENT);
        return;
    }

    num = malloc(strlen(p_copy));
    check_allocation(num);
    strcpy(num,p_copy);

    /* If the array is equal to 0, it is int 0. else set the int to atoi value */
    if (*num == '0'){
        int len = strlen(num);
        int i;
        /* Traverse in the list until we reach a character that is different than 0, or the end of the string */
        for (i=0; i< len; i++){
            if (*num != '0'){
                if ((strToInt(num)) == 9000){
                    error(ERR_UNDEFINED_ARGUMENT);
                    return;
                }
                else
                    break;
            }
            num++;
        }
        p_copy = malloc(strlen(num));
        strcpy(p_copy,num);

        /* Check there is no more data */
        num = strtok(NULL," \t\n\r\f\v");
        if (num != NULL){
            error(ERR_EXTRANEOUS_TEXT);
            return;
        }
        if (temp != NULL)
            add_label(curr_line_number,strToInt(p_copy),label_name,1,label_type);
        else
            add_label(curr_line_number,strToInt(p_copy),label_name,0,label_type);
        return;
    }

    else if ((strToInt(num)) == 9000){
        error(ERR_UNDEFINED_ARGUMENT);
        return;
    }
    
    /* Check there is no more data */
    p_copy = strtok(NULL," \t\n\r\f\v");
    if (p_copy != NULL){
        error(ERR_EXTRANEOUS_TEXT);
        return;
    }

    /* Add the label */
    if (temp != NULL){                  /* If there is a duplicate, change the count to 1 */
            add_label(curr_line_number,atoi(num),label_name,1,label_type);
            return;
    }
    else 
        add_label(curr_line_number,atoi(num),label_name,0,label_type);
    return;
}

/*
*   Handles the second case of define data input. case 2:  .define label =num
*/
void def_case_2(char *pointer, Label_Type label_type, char *pointer2){
    label_node *temp = NULL;
    char *label_name = NULL;
    char *num;
    char *p_copy = pointer;                                     /* Copies the pointer */
    /* Check the label */
    if (!check_label(p_copy,label_type)){
        return;    
    }
    
    /* Save the label in label name variable */
    label_name = malloc(strlen(p_copy));
    check_allocation(label_name);
    strcpy(label_name,p_copy);

    /* Grab the duplicate if it exists */
    temp = label_exists(label_name);

    /* Check there is no more data */
    pointer2 = strtok(pointer2," \t\n\v\r\f");
    if (pointer2 == NULL){
        error(ERR_MISSING_ARGUMENT);
        return;
    }

    /* Save the variable in num string for int */
    num = malloc(strlen(pointer2));
    check_allocation(num);
    strcpy(num,pointer2);

    /* If the array is equal to 0, it is int 0. else set the int to strToInt value */
   if (*num == '0'){
        int len = strlen(num);
        int i;
        for (i=0; i< len; i++){
            if (*num != '0'){
                if ((strToInt(num)) == 9000){
                    error(ERR_UNDEFINED_ARGUMENT);
                    return;
                }
                else
                    break;
            }
            num++;
        }
        p_copy = malloc(strlen(num));
        strcpy(p_copy,num);
        
        /* Check there is no more data */
        num = strtok(NULL," \t\n\r\f\v");
        if (num != NULL){   
            error(ERR_EXTRANEOUS_TEXT);
            return;
        }
        if (temp != NULL)
            add_label(curr_line_number,strToInt(p_copy),label_name,1,label_type);
        else 
            add_label(curr_line_number,strToInt(p_copy),label_name,0,label_type);
        return;
    }
    else if ((strToInt(num)) == 9000){
        error(ERR_UNDEFINED_ARGUMENT);
        return;
    }

    /* Check there is no more data */
    p_copy = pointer2;
    p_copy = strtok(NULL," \t\n\r\f\v");
    if (p_copy != NULL){
        error(ERR_EXTRANEOUS_TEXT);
        return;
    }

    /* Add the label */
    if (temp != NULL){                  /* If there is a duplicate, change the count to 1 */
        add_label(curr_line_number,atoi(num),label_name,1,label_type);
        return;
    }
    else 
        add_label(curr_line_number,atoi(num),label_name,0,label_type);
    return;
}

/*
*   Handles the third case of define data input. case 3:  .define label= num
*/
void def_case_3(char *pointer, Label_Type label_type ,char *pointer2){
    label_node *temp = NULL;
    char *label_name = NULL;
    char *num;
    char *p_copy = pointer;                                     

    /* Check the label */
    if (!check_label(p_copy,label_type)){
        return;    
    }
    
    /* Save the label in label name variable */
    label_name = malloc(strlen(p_copy));
    check_allocation(label_name);
    strcpy(label_name,p_copy);
    
    /* Grab the duplicate if it exists */
    temp = label_exists(label_name);

    /* Save the variable in num string for int */
    num = malloc(strlen(pointer2));
    check_allocation(num);
    strcpy(num,pointer2);

    /* If the array is equal to 0, it is int 0. else set the int to strToInt value */
    if (*num == '0'){
        int len = strlen(num);
        int i;
        for (i=0; i< len; i++){
            if (*num != '0'){
                if ((strToInt(num)) == 9000){
                    error(ERR_UNDEFINED_ARGUMENT);
                    return;
                }
                else
                    break;
            }
            num++;
        }
        p_copy = malloc(strlen(num));
        strcpy(p_copy,num);

        /* Check there is no more data */
        num = strtok(NULL," \t\n\r\f\v");
        if (num != NULL){
            error(ERR_EXTRANEOUS_TEXT);
            return;
        }

        /* Add the label */
        if (temp != NULL)                  
            add_label(curr_line_number,atoi(num),label_name,1,label_type);
        else 
            add_label(curr_line_number,atoi(num),label_name,0,label_type);
        return;
    }

    else if ((strToInt(num)) == 9000){
        error(ERR_UNDEFINED_ARGUMENT);
        return;
    }

     /* Check there is no more data */
    p_copy = pointer2;
    p_copy = strtok(NULL," \t\n\r\f\v");
    if (p_copy != NULL){
        error(ERR_EXTRANEOUS_TEXT);
        return;
    }

    /* Add the label */
    if (temp != NULL)                  
        add_label(curr_line_number,atoi(num),label_name,1,label_type);
    else 
        add_label(curr_line_number,atoi(num),label_name,0,label_type);
    return;

}

/*
*   Handles the fourth case of define data input. case 4:  .define label = num
*/
void def_case_4(char *pointer, Label_Type label_type ,char *pointer2){
    label_node *temp = NULL;
    char *label_name = NULL;
    char *num;
    char *p_copy = pointer;                                     /* Copies the pointer */
    /* Check the label */
    if (!check_label(p_copy,label_type)){
        return;    
    }
    
    /* Save the label in label name variable */
    label_name = malloc(strlen(p_copy));
    check_allocation(label_name);
    strcpy(label_name,p_copy);

    /* Grab the duplicate if it exists */
    temp = label_exists(label_name);

    /* Save the variable in num string for int */
    num = malloc(strlen(pointer2)+1);
    check_allocation(num);
    strcpy(num,pointer2);

    /* If the array is equal to 0, it is int 0. else set the int to atoi value */
    if (*num == '0'){
        int len = strlen(num);
        int i;
        for (i=0; i< len; i++){
            if (*num != '0'){
                if ((strToInt(num)) == 9000){
                    error(ERR_UNDEFINED_ARGUMENT);
                    return;
                }
                else
                    break;
            }
            num++;
        }
        p_copy = malloc(strlen(num));
        strcpy(p_copy,num);

        /* Check there is no more data */
        num = strtok(NULL," \t\n\r\f\v");
        if (num != NULL){
            error(ERR_EXTRANEOUS_TEXT);
            return;
        }

        /* Add the label */
        if (temp != NULL)                  
            add_label(curr_line_number,atoi(num),label_name,1,label_type);
        else 
            add_label(curr_line_number,atoi(num),label_name,0,label_type);
        return;
    }

    else if (strToInt(num) == 9000){
        error(ERR_UNDEFINED_ARGUMENT);
        return;
    }
    
    /* Check there is no more data */
    p_copy = pointer2;
    p_copy = strtok(NULL," \t\n\r\f\v");
    if (p_copy != NULL){
        error(ERR_EXTRANEOUS_TEXT);
        return;
    }

    /* Add the label */
    if (temp != NULL){                  /* If there is a duplicate, change the count to 1 */
        add_label(curr_line_number,atoi(num),label_name,1,label_type);
        return;
    }
    else 
        add_label(curr_line_number,atoi(num),label_name,0,label_type);
    return;
}
