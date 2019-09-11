//main.c
#include <stdio.h>
#include <fcntl.h>
#include "simulator/simulator.h"

int main(int argc, char** argv){

    if(argc != 2 || argv[1] == NULL){
        printf("Binary file not supplied.");
        return -1;
    }

    char* filename = argv[1];
    int binary_fd;

    // Open the provided file
    binary_fd = open(filename, O_RDONLY);

    if(binary_fd < 0){
        printf("Error reading binary file: %d\n", binary_fd);
        return -1;
    }

    close(binary_fd);

    return 0;
}