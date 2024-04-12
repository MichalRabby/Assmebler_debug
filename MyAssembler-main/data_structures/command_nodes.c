#include "../include/assembler.h"

cmd_node *create_cmd_node(int cmd_num) {
    cmd_node *result = malloc(sizeof(cmd_node));
    cmd_node *temp = cmd_head;
    check_allocation(result);
    result -> cmd_num = cmd_num;
    result -> next_cmd = NULL;
    result-> address = IC;
    result-> L = 1;
    result-> cmd_binary = malloc(BIN_WORD_LEN+1);
    check_allocation(result->cmd_binary);
    /*result-> source1_binary = malloc(BIN_WORD_LEN+1);
    //check_allocation(result->source1_binary);
    //result-> source2_binary = malloc(BIN_WORD_LEN+1);
    //check_allocation(result->source2_binary);
    //result-> target1_binary = malloc(BIN_WORD_LEN+1);
    //check_allocation(result->target1_binary);
    //result-> target2_binary = malloc(BIN_WORD_LEN+1);
    //check_allocation(result->target2_binary);
    //result-> source_label = malloc(MAX_LABEL_NAME) ;
    //check_allocation(result->source_label);
    //result-> target_label = malloc(MAX_LABEL_NAME) ;
    //check_allocation(result->target_label);
    /* connect new cmd to the end of the cmd list */
    if (cmd_head == NULL ){/* list is empty */
        cmd_head = result;    
    }
    else {
        while(temp -> next_cmd != NULL ){
            temp = temp -> next_cmd;
        }
        temp->next_cmd = result;
    }
    

    /*result -> next_label = NULL;*/    
    result -> curr_line = curr_line_number;
    return result;
}

/* this function gets a binary string, label name and index value and creates a ins_node with these values. */
ins_node *create_ins_node(char *bin_val, char *label_name, int index, int addressing){
    ins_node *result = malloc(sizeof(ins_node));
    check_allocation(result);
    result-> bin = malloc(BIN_WORD_LEN+1);
    check_allocation(result-> bin);
    strcpy(result-> bin, bin_val);
    result-> label_name = malloc(MAX_LABEL_NAME);
    check_allocation(result->label_name);
    if (label_name != NULL)
        strcpy((result-> label_name), label_name);
    result-> index = index ;
    result-> addressing_method = addressing;
    result-> next = NULL ;
    return result;
}

ins_node *add_ins_node(cmd_node *cur_cmd ,char *bin_val, char *label_name, int index, int addressing){
    ins_node *temp = cur_cmd-> next_ins ;
    ins_node *result = create_ins_node(bin_val, label_name, index, addressing);
    result->L = cur_cmd-> L ; 
    if (temp == NULL){ /* list is empty */
        cur_cmd -> next_ins = result ;
        return result ;
    }
    else
        while (temp-> next != NULL) {
            temp = temp-> next ;
        }
    temp-> next = result ;
    return result ;    
}

