#include "util.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//converts each byte of the buffer s to its corresponding pair of hexadecimal
//digits and prints it to stdout; eg. "input "AAAA" -> output "41 41 41 41
void binary_to_hex(char *s, int n){
//creating an integer to count bytes read
int byteCounter = 0;
//accessing each char in the char*
    for (int i=0; i<n; i++){
        byteCounter+=1;
        //printing every character of the string as hexadecimal
        printf ("%x ", *(s+i));
        //creating a new line every 16 bytes
        if (byteCounter%16 == 0){
            printf ("\n");
        }
    }
}

//This function takes a null-terminated buffer containing valid hexadecimal
//characters and returns a pointer to a buffer containing the corresponding
//binary; eg. input "68 65 6c 6c 6f" -> output "hello"
char *hex_to_binary(char *s){
    int lengthOfInput = strlen(s);
    //counting number of spaces
    int numberOfSpaces = 0;
    for (int i=0; i<lengthOfInput;i++){
        if (isspace(*(s+i))){
            numberOfSpaces++;
        }
    }
    int lengthOfInputWithoutSpaces = lengthOfInput - numberOfSpaces;
    //removing spaces from the string and creating a new string
    char* str = malloc (lengthOfInputWithoutSpaces);
    int spacesToIgnore = 0;
    for (int i=0; i<lengthOfInput; i++){
        if (isspace(*(s+i))){
            spacesToIgnore++;
        }
        else{
            str [i-spacesToIgnore] = *(s+i);
        }
    }
    //checking that all the characters are hex characters
    for (int i=0;i<lengthOfInputWithoutSpaces;i++){
        if (!(isxdigit(*(str+i)))){
            free(str);
            return NULL;
        }
    }
    char* byte = malloc (4);
    byte[0]='0';
    byte[1]='x';
    int lengthOfBinaryString = lengthOfInputWithoutSpaces/2;
    //requesting memory for a binary string
    char* binaryString = malloc (lengthOfBinaryString);
    //getting each pair of hex digits that form a byte
    int byteCount = 0;
    for (int i=0; i<lengthOfInputWithoutSpaces; i++){
         if (i%2==0){
             memcpy (byte+2, str+i, 2);
             //converting each hex string to a hex integer
             int numEquiv = (int) strtol(byte, NULL, 16);
             //adding them as characters to binary string
             binaryString[byteCount++] = numEquiv;
         }
    }
    return binaryString;
}
