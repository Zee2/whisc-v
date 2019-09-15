//main.c
#include <stdio.h>
#include <fcntl.h>
#include "simulator/simulator.h"
#include "simulator/opcodes.h"
#include "simulator/decode.h"

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

    // Open the provided file
    binary_file = fopen(filename, "rb");

    if(binary_file == NULL){
        perror("Error reading binary file: ");
        return -1;
    }

    uint16_t current_instruction;


    while(1){
        int count = fread(&current_instruction, 4, 1, binary_file);
        if(count == 0){ break; }
        
        printf("%2x", (current_instruction >> 24) & 0xFF);
        printf("%2x", (current_instruction >> 16) & 0xFF);
        printf("%2x", (current_instruction >> 8) & 0xFF);
        printf("%2x : ", (current_instruction >> 0) & 0xFF);
        instruction_rv32i_t instruction;
        if(decode_rv32i(current_instruction, &instruction) < 0){
            printf("Error!\n");
            continue;
        } else {
            pretty_print_rv32i(instruction);
        }
        

    }
    

    fclose(binary_file);

    return 0;
}