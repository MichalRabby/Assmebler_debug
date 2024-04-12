#include "../include/assembler.h"
 
void error_manager(ErrorCode errorCode) {
    if (err_flag == 0){
        fprintf(stderr, "Printing errors for file: %s\n",curr_file);
    }
    if(errorCode == ERR_ERR_FLAG){
        fprintf(stderr, "You have [%d] errors in file [%s], continuing to next file!\n\n",err_flag,curr_file);
        return;
    }
    err_flag++;
    switch (errorCode) {
        case ERR_UNDEFINED_REGISTER:
            fprintf(stderr, "Error: Undefined register name in line %d\n", curr_line_number);
            break;
        case ERR_UNDEFINED_COMMAND:
            fprintf(stderr, "Error: Undefined command name in line %d\n", curr_line_number);
            break;
        case ERR_EXTRANEOUS_TEXT:
            fprintf(stderr, "Error: Extraneous text after end of command in line %d\n", curr_line_number);
            break;
        case ERR_UNDEFINED_ARGUMENT:
            fprintf(stderr, "Error: Undefined argument in line %d\n", curr_line_number);
            break;
        case ERR_MISSING_ARGUMENT:
            fprintf(stderr, "Error: Missing argument in line %d\n", curr_line_number);
            break;
        case ERR_ILLEGAL_COMMA:
            fprintf(stderr, "Error: Illegal comma in line %d\n", curr_line_number);
            break;
        case ERR_MISSING_COMMA:
            fprintf(stderr, "Error: Missing comma in line %d\n", curr_line_number);
            break;
        case ERR_MISSING_PARENTHESES:
            fprintf(stderr, "Error: Missing parentheses in line %d\n", curr_line_number);
            break;
        case ERR_MULTIPLE_CONSECUTIVE_COMMAS:
            fprintf(stderr, "Error: Multiple consecutive commas in line %d\n", curr_line_number);
            break;
        case ERR_SEGMENTATION_FAULT:
            fprintf(stderr, "Error: Segmentation fault in line %d\n", curr_line_number);
            break;
        case ERR_REDEFINITION_MACRO:
            fprintf(stderr, "Error: Redefinition of macro name in line %d\n", curr_line_number);
            break;
        case ERR_ILLEGAL_ADDRESSING:
            fprintf(stderr, "Error: Illegal addressing method in line %d\n", curr_line_number);
            break;
        case ERR_IMM_OVERFLOW:
            fprintf(stderr, "Error: Immidiate overflow in line %d\n", curr_line_number);
            break;
        case ERR_SIZE_LEAK:
            fprintf(stderr, "Error: Line [%d] is over 80 characters long \n", curr_line_number);
            break;
        case ERR_DUPLICATE_LABEL:
            fprintf(stderr, "Error: Label in line [%d] is a duplicate! \n", curr_line_number);
            break;
        case ERR_INVALID_LABEL:
            fprintf(stderr, "Error: Invalid Label name in line %d\n", curr_line_number);
            break;
        case ERR_FILE_NAME:
            fprintf(stderr, "Error: No such file %s to open!\n",curr_file);
            break;
        case ERR_FILE_ARGS:
            fprintf(stderr, "Error: Please input file names to read from!\n");
            break;
        case ERR_LBL_PARTNER:
            fprintf(stderr, "Error: Entry in line %d Does not have a decleration in the file!\n",curr_line_number);
            break;
        case ERR_UNDEFINED_LBL_USE:
            fprintf(stderr, "Error: Undefined Label is used in line %d\n",curr_line_number);
            break;
        default:
            fprintf(stderr, "Error: Unknown error in line %d\n", curr_line_number);
            break;
    }
}
   
