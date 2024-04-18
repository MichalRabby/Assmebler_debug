#include "../include/assembler.h"


/*
*   This function creates a word_node according to the label name, type, initiated address and sets everything else to NULL
*/
label_node *create_label(int line_init,int definedData,char *label_name,int entry_count,Label_Type label_type){
    label_node *new_label = malloc(sizeof(label_node));                        /* Initialize room for a new label node */
    check_allocation(new_label);
    new_label -> label_name = malloc(strlen(label_name) + 1);                         /* Initialize room for a new label string */
    check_allocation(new_label -> label_name);
    strcpy(new_label -> label_name, label_name);                                      /* Store a copy of the label name */

    new_label -> line_init = line_init;
    new_label -> label_type = label_type;
    new_label -> definedData = definedData;
    new_label -> entry_count = entry_count;
    new_label -> data_count = 0;
    new_label -> data_node = NULL;
    new_label -> row_list = NULL;
    new_label -> next_entry = NULL;
    new_label -> next_extern = NULL;
    new_label -> next_dc = NULL;
    new_label -> next_cmd_label = NULL;
    new_label -> next_label = NULL;
    new_label -> prev_label = NULL;
    return new_label;
}

/*
*   This function adds a label to the list
*/
void *add_label (int line_init,int definedData,char *label_name,int entry_count,Label_Type label_type) {
    label_node *new_label = create_label(line_init, definedData, label_name, entry_count, label_type);                           /* Creates a new node */
    label_node *temp = lbl_head; 
    if (lbl_head == NULL) {                                                   /* If the list is empty, add the new node to the top of the list */
        lbl_head = new_label;
    }
    else {
        while (temp -> next_label != NULL) {
            temp = temp -> next_label;      
        }  
        temp -> next_label = new_label;
        new_label -> prev_label = temp;  
    }
    if (new_label -> label_type == EXTERN_LABEL)
        add_extern(new_label);
    else if (new_label -> label_type == ENTRY_LABEL)
        add_entry(new_label);
    else if (new_label -> label_type == DATA_LABEL || new_label -> label_type == STRING_LABEL)
        add_dc(new_label);
    else if (new_label -> label_type == CMD_LABEL)
        add_cmd_label(new_label);    
    
    return new_label;
}


/*
*   This function checks if a label exists in the data list 
*/
label_node *label_exists(char *curr_label) {
    char *copy = NULL;
    label_node *temp = NULL;
    copy = malloc(strlen(curr_label)+1);
    check_allocation(copy);
    strcpy(copy,curr_label);
    temp = lbl_head;
    if (lbl_head==NULL)
        return NULL;                                                                          /* Returns if the list is empty */
    while (temp != NULL) {
        if (strcmp(temp -> label_name,copy) == 0)                                             /* Goes through the list and compares the names */
            return temp;
        temp = temp -> next_label; 
    }
    return NULL;
}


/*
*   This function adds a an entry node to the entry list
*/
void *add_entry(label_node *some_node) {
    label_node *temp = entry_head;
    if (entry_head == NULL) {                                                   /* If the list is empty, add the new node to the top of the list */
        entry_head = some_node;
    }
    else {
        while (temp -> next_entry != NULL) {
            temp = temp -> next_entry;      
        }  
        temp -> next_entry = some_node;
    }
    return temp;
}

/*
*   This function adds a an external node to the extern list
*/
void *add_extern(label_node *some_node) {
    label_node *temp = extern_head; 
    if (extern_head == NULL) {                                                   /* If the list is empty, add the new node to the top of the list */
        extern_head = some_node;
    }
    else {
        while (temp -> next_extern != NULL) {
            temp = temp -> next_extern;      
        }  
        temp -> next_extern = some_node;
    }
    return temp;
}

/*
*   This function adds a dc node to the dc list
*/
void *add_dc(label_node *some_node){
    label_node *temp = dc_head; 
    if (dc_head == NULL) {                                                   /* If the list is empty, add the new node to the top of the list */
        dc_head = some_node;
    }
    else {
        while (temp -> next_dc != NULL) {
            temp = temp -> next_dc;      
        }  
        temp -> next_dc = some_node;
    }
    return (void*)temp;
}

/*
*   This function adds a cmd node to the cmd list
*/
void *add_cmd_label(label_node *some_node){
    label_node *temp = cmd_label_head; 
    if (cmd_label_head == NULL) {                                                   /* If the list is empty, add the new node to the top of the list */
        cmd_label_head = some_node;
    }
    else {
        while (temp -> next_cmd_label != NULL) {
            temp = temp -> next_cmd_label;      
        }  
        temp -> next_cmd_label = some_node;
    }
    return temp;
}



/*
*   This function receives a variable and prints the coresponding list. 
*   Enter 1 for label list, 2 for dc list, and 3 for instruction list.
*/
void printList(int num){
    label_node *temp = NULL;
    cmd_node *cmd_temp = NULL;
    data_node *data_temp = NULL; 
    row_node *row_temp = NULL; 
    label_node *dc_temp = NULL;
    printf("\n");
    switch (num){
        case 1: /* label list */
            print_label_guide();
            printf("********************************************\n");
            printf("\t\tPrinting label tabel\n");
            printf("********************************************\n");
            temp = lbl_head;
            while (temp != NULL){
                switch (temp -> label_type){
                    case EXTERN_LABEL:
                        printf("[%s]-[%d]-[%d]-",temp -> label_name,temp -> label_type,temp -> line_init);
                        row_temp = temp -> row_list;
                        if (row_temp == NULL){
                            printf("NONE\n");
                        }
                        else{
                            while (row_temp != NULL){
                                if (row_temp -> next_row == NULL)
                                    printf("[%d]\n",row_temp -> address);
                            else
                                printf("[%d]-",row_temp -> address);
                            row_temp = row_temp -> next_row;
                            }
                        }
                        break;
                    case DEF_LABEL:
                        printf("[%s]-[%d]-[%d]\n",temp -> label_name,temp -> label_type,temp -> definedData);
                        break;
                    case ENTRY_LABEL:
                        printf("[%s]-[%d]-[%d]\n",temp -> label_name,temp -> label_type,temp -> entry_count);
                        break;
                    case CMD_LABEL:
                        printf("[%s]-[%d]-[%d]\n",temp -> label_name,temp -> label_type,temp -> line_init);
                        break;
                    default:
                        printf("[%s]-[%d]-[%d]-",temp -> label_name,temp -> label_type,temp -> line_init);
                        data_temp = temp -> data_node;
                        while (data_temp != NULL){
                            if (data_temp -> next_data == NULL)
                                printf("[%d]\n",data_temp -> data);
                            else
                                printf("[%d]-",data_temp -> data);
                            data_temp = data_temp -> next_data;
                        }
                        break;
                }
                if (temp -> next_label != NULL)
                    printf("  |\n");
                else    
                    printf("\n");
                temp = temp -> next_label;
            }
            break;
        case 2: /* dc list */
            dc_temp = dc_head;
            printf("********************************************\n");
            printf("\t\tPrinting DC list\n");
            printf("********************************************\n");
            while (dc_temp != NULL){
                printf("[%s]-[%d]-[%d]-",dc_temp -> label_name,dc_temp -> label_type,dc_temp -> line_init);
                data_temp = dc_temp -> data_node;
                while (data_temp != NULL){
                    if (data_temp -> next_data == NULL)
                        printf("[%d]\n",data_temp -> data);
                    else
                        printf("[%d]-",data_temp -> data);
                    data_temp = data_temp -> next_data;
                    }
                if (dc_temp -> next_dc != NULL)
                    printf("  |\n");
                else    
                    printf("\n");
                dc_temp = dc_temp -> next_dc;
            }
            
            break;
        /*case 3: /* cmd list 
            cmd_temp = cmd_head;
            printf("********************************************\n");
            printf("\t\tPrinting CMD list\n");
            printf("********************************************\n");
            while (cmd_temp != NULL){
                if (cmd_temp -> next_cmd == NULL){
                   printf("cmd name: %s | cmdBin[%s]-s1Bin-[%s]-s2Bin-[%s]-t1Bin-[%s]-t2Bin-[%s]\n",commands[cmd_temp -> cmd_num] ,cmd_temp -> cmd_binary,(cmd_temp -> source1_binary),(cmd_temp -> source2_binary),(cmd_temp -> target1_binary),(cmd_temp -> target2_binary)); 
                }
                else 
                   printf("cmd name: %s | cmdBin[%s]-s1Bin-[%s]-s2Bin-[%s]-t1Bin-[%s]-t2Bin-[%s]\n",commands[cmd_temp -> cmd_num] ,cmd_temp -> cmd_binary,(cmd_temp -> source1_binary),(cmd_temp -> source2_binary),(cmd_temp -> target1_binary),(cmd_temp -> target2_binary));
                printf("\n");
                cmd_temp = cmd_temp -> next_cmd;
            }
            break;*/
        default:
            break;
    }
    return;
}

/*
*   This function frees the dynamicaly allocated nodes in the list.
*   Enter 1 for label list, 2 for dc list, and 3 for instruction list.
*/
void freeLists() {
    label_node *current_label = lbl_head;
    label_node *temp_label = NULL;
    row_node *current_row = NULL;
    row_node *temp_row = NULL;
    data_node *current_data = NULL;
    data_node *temp_data = NULL;
    cmd_node *current_cmd = cmd_head;
    cmd_node *temp_cmd = NULL;
    /* Free the label tabel */
    while (current_label != NULL){
        /* If we have a data or string label, we need to free the data nodes */
        if (current_label -> label_type == DATA_LABEL || current_label -> label_type == STRING_LABEL){
            current_data = current_label -> data_node;
            while (current_data != NULL){
                temp_data = current_data -> next_data;
                free(current_data);
                current_data = temp_data;
            }
        }
        /* If we have an external label, we need to free the row nodes */
        else if (current_label -> label_type == EXTERN_LABEL){
            current_row = current_label -> row_list;
            while (current_row != NULL){
                temp_row = current_row -> next_row;
                free(current_row);
                current_row = temp_row;
            }
        }
        temp_label = current_label->next_label;
        free(temp_label);
        current_label = temp_label;
    }
    /* Free the command nodes */
    while (current_cmd != NULL){
        temp_cmd = current_cmd->next_cmd;
        free(current_cmd);
        current_cmd = temp_cmd;
    }
}

void print_label_guide() {
    printf("-----------------------------------------------------------------\n");
    printf("| Label Name | Label Type   | Details                           |\n");
    printf("-----------------------------------------------------------------\n");
    printf("| LabelName  | CMD_LABEL(0)   | LineInit                        |\n");
    printf("| LabelName  | DEF_LABEL(1)   | DefinedData                     |\n");
    printf("| LabelName  | ENTRY_LABEL(2) | EntryCount                      |\n");
    printf("| LabelName  | EXTERN_LABEL(3)| LineInit| Addresses: Address1...|\n");   
    printf("| LabelName  | STRING_LABEL(4)| DC | Data: Data1, ...           |\n");
    printf("| LabelName  | DATA_LABEL(5)  | DC | Data: Data1, ...           |\n");
    printf("-----------------------------------------------------------------\n");
}

