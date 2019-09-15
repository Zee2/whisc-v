//main.c
#include <stdio.h>
#include <fcntl.h>
#include "simulator/simulator.h"
#include "simulator/opcodes.h"
#include "simulator/decode.h"
#include "simulator/core.h"

uint32_t program_data[1024];

int main(int argc, char** argv){

    if(argc != 2 || argv[1] == NULL){
        printf("Binary file not supplied.");
        return -1;
    }

    char* filename = argv[1];
    FILE* binary_file;

    // Open the provided file
    binary_file = fopen(filename, "rb");

    if(binary_file == NULL){
        perror("Error reading binary file: ");
        return -1;
    }


    core_state_t processor_state;
    for(int i = 0; i < REGFILE_SIZE; i++){
        processor_state.regfile[i] = i;
    }
    processor_state.pc_reg = 0;

    uint32_t current_instruction;

    
    fread(&program_data, 4, 32, binary_file);

    for(int j = 0; j < 8; j++){
            printf("  x%d: %d", j, processor_state.regfile[j]);

            if(j % 2 != 0 && j != 0) printf("\n");
        }

    for(int i = 0; i < 12; i++){
        int result = execute_rv32i(program_data, &processor_state, &processor_state);

        if(result != 0){
            printf("Error!\n");
            break;
        }

        for(int j = 0; j < 8; j++){
            printf("  x%d: %d", j, processor_state.regfile[j]);

            if(j % 2 != 0 && j != 0) printf("\n");
        }
        

    }
    

    fclose(binary_file);

    return 0;
}