#include "../include/assembler.h"

/*
*   This function creates a data_node
*/
data_node *create_data(int data, label_node *label_node){
    data_node *new_data = malloc(sizeof(data_node));                        /* Initialize room for a new data node */
    check_allocation(new_data);
    new_data -> data = data;                                                            
    new_data -> next_data = NULL;

    return new_data;
}

/*
*   This function adds a data node to the data list in a label 
*/
void *add_data (int data,label_node *label_node){
    data_node *new_data = create_data(data,label_node);
    data_node *current = label_node -> data_node;

    if (label_node -> data_node == NULL) {
        label_node -> data_node = new_data;  
    } else {
        while (current -> next_data != NULL) {
            current = current -> next_data;
        }
        current -> next_data = new_data;
    }

    (label_node -> data_count)++;
    
    return (void*)new_data;
}
