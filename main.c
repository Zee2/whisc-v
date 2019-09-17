//main.c
#include <stdio.h>
#include <fcntl.h>
#include "simulator/simulator.h"
#include "simulator/opcodes.h"
#include "simulator/decode.h"
#include "simulator/core.h"

memory_t main_memory = {
    mem_lower_bound: 0,
    mem_upper_bound: MEM_SIZE-1
};

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

    fread(&(main_memory.data), 1, MEM_SIZE, binary_file);

    for(int j = 0; j < 8; j++){
            printf("  x%d: %d", j, processor_state.regfile[j]);
            if(j % 2 != 0 && j != 0) printf("\n");
        }

    for(int i = 0; i < 12; i++){
        int result = execute_rv32i(&main_memory, &processor_state, &processor_state);

        if(result != 0){
            printf("Error!\n");
            break;
        }

        for(int j = 0; j < 8; j++){
            printf("  x%d: %04x", j, processor_state.regfile[j]);
            if(j % 2 != 0 && j != 0) printf("\n");
        }

        printf("Press any key to continue\n");
        getchar();
        

    }
    

    fclose(binary_file);

    return 0;
}