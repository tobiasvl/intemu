#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "bus.h"

//  Memset bus memory
//  cpu memset 0 
//  set pc til 0x100
//  lag read from file
//  lag fetch byte
//  lag fetch uint16
//  lag decoder uint8

int decode_opcode(uint8_t opcode)
{
    switch (opcode) {
        case 0x000:
            //fprintf(stderr, "0x0000 ");
            break;
        case 0x001:
            fprintf(stderr, "0x0001 ");
            // FOO
            break;
        case 0x002:
            fprintf(stderr, "0x0002 ");
            // BAR
            break;
        default:
            // code block
    }

    return 0;

}

int fetch_byte(S_CPU *cpu, S_BUS *bus)
{

    return 0;
}

int fetch_uint16(S_CPU *cpu, S_BUS *bus)
{

    return 0;
}


int fetch(S_CPU *cpu, S_BUS *bus)
{
    while (true)
    {
        uint8_t byte = bus->memory[cpu->PC];
        decode_opcode(byte);

        //Need an exit path
        // if something then exit
        cpu->PC += 1;
    }

    return 0;
}


int cpu_init(uint8_t* file_buffer, ssize_t file_size)
{
    fprintf(stderr, "[*] Entering CPU_INIT\n");
    S_CPU cpu;
    S_BUS bus;

    memset(bus.memory, 0, MAX_MEMORY_SIZE);

    if (file_size <= MAX_MEMORY_SIZE) {
        memcpy(bus.memory, file_buffer, file_size);
    } else {
        fprintf(stderr, "Warning: File size exceeds memory size. Only copying the first %d bytes.\n", MAX_MEMORY_SIZE);
        memcpy(bus.memory, file_buffer, MAX_MEMORY_SIZE);
    }

    cpu.PC = 0x100;
    fetch(&cpu, &bus);

    return 0;

}
