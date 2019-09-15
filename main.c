//main.c
#include <stdio.h>
#include <fcntl.h>
#include "simulator/simulator.h"
#include "simulator/opcodes.h"

int main(int argc, char** argv){

    if(argc != 2 || argv[1] == NULL){
        printf("Binary file not supplied.");
        return -1;
    }

    CB_type_t cb_var;

    cb_var.offset3 = 4;

    char* filename = argv[1];
    FILE* binary_file;

    printf("Hello!\n");

    //int result = decode_compressed();

    printf("Hello!\n");

    // Open the provided file
    binary_file = fopen(filename, "rb");

    if(binary_file == NULL){
        perror("Error reading binary file: ");
        return -1;
    }

    uint16_t current_instruction;


    while(1){
        int count = fread(&current_instruction, 2, 1, binary_file);
        if(count == 0){ break; }
        printf("Instruction: ");
        printf("%x", current_instruction >> 8);
        printf("%x\n", current_instruction & 0xFF);
    }
    

    fclose(binary_file);

    return 0;
}