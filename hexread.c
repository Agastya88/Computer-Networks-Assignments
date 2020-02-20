#include "util.c"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    FILE* file = fopen (argv[1], "r");
    //if no file exists with that name
    if (file==NULL){
        if (argv[1]==NULL){
            printf ("Input: ");
            char *str = malloc (4096);
            fgets(str, 4096, stdin);
            char* result = hex_to_binary (str);
            printf ("%s\n", result);
        }
        else{
            char* result = hex_to_binary (argv[1]);
            printf ("%s\n", result);
        }
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
        printf ("Filesize: %d\n", fileSize);
        char* str = malloc (fileSize);
        fread (str, 1, fileSize, file);
        char* result = hex_to_binary (str);
        printf ("%s\n", result);
    }
}
