#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "util.h"

int main(int argc, char *argv[]) {
    //if there is an argument, it should be a file
    if (argv[1]!=NULL){
        FILE* file = fopen (argv[1], "r");
        //if no file exists with that name
        if (file==NULL){
            printf ("Error: Invalid File Name\n");
        }
        //if a file does exist with that name
        else{
            struct stat st;
            int fileSize = 0;
            //getting the file size
            if(stat(argv[1],&st)==0){
                fileSize = st.st_size;
            }
            //error checking
            else{
                return -1;
            }
            char* str = malloc (fileSize);
            fread (str, 1, fileSize, file);
            binary_to_hex (str, strlen(str));
            printf ("\n");
        }
    }
    //if there is no argument, it should take input from stdin
    else{
        char *str = malloc (4096);
        fgets(str, 4096, stdin);
        binary_to_hex (str, strlen(str));
        printf ("\n");
    }
}
