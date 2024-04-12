#include "../include/assembler.h"

/* this function gets a obj file pointer and a binary string. it tranlates the binary code to encrypted 4 base code and writes the line in the obj file */
void binToFour(FILE *obj_fp, char *str) {
    int i;
        for (i=0; i<BIN_WORD_LEN; i=i+2) {
            if (*(str+i) == '0') {
                if (*(str+i+1)=='0') { /* 00 */
                    fputc('*',obj_fp);
                    continue;
                }
                else {/* 01 */
                    fputc('#',obj_fp);
                    continue;
                }
            }
            else {
                if (*(str+i+1)=='0') { /* 10 */
                    fputc('%',obj_fp);
                    continue;
                }
                else {/* 11 */
                    fputc('!',obj_fp);
                    continue;
                }
            }
        }
    fputc('\n',obj_fp);
    str++; 
}

/* 
*   This function checks to see if a targetWord exists in a word_array
*   Returns 1 if it is, 0 if it isnt.
*/
int checkWordInArray(char **word_array, char *targetWord){
    int i;
    for (i = 0; word_array[i] != NULL; i++){
        if (strcmp(word_array[i], targetWord) == 0){
            return 1; 
        }
    }   
    return 0;
}

/*
*   Calculates the given string to an int value.
*   If the int calculated is a greater value than what can be displayed with 14 bits in signed magnitude, 
*   Or if the string is not entirely numbers, returns 9000
*/
int strToInt(char *string){
    int len,i = 0;
    int flag = 0;
    if (string[0] == '-'){
        flag = -1;
    }
    else if (string[0] == '+'){
        flag = 1;
    }
    if (flag != 0)
        string = string+1;
    else 
        flag = 1;
    len = strlen(string);
    for (i = 0; i < len; i++){
        if (string[i] > 57 || string[i] < 48)
            return 9000;
    }

    
    if (flag*atoi(string) > 8191 || flag*atoi(string) < -8191)
        return 9000;
    return flag*atoi(string);

}

/*
*   This function translates a number to a string of 14 binary 
*/
char *BinTranslation14Bit(int num) {
    static char result[BIN_WORD_LEN];
    int i, bit, index = 0;
    int negHandle = 1 << BIN_WORD_LEN; 

    /* Handle negative numbers using Two's complement */
    if (num < 0) {
        num += negHandle;
    }

    /* Store the binary representation of num in the string */
    for (i = BIN_WORD_LEN-1; i >= 0; i--) {
        bit = (num >> i) & 1;
        result[index] = bit + '0'; 
        index++;
    }
    return result;
}

