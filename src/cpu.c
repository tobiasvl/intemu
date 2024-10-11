#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "bus.h"

//  DONE - Memset bus memory
//  DONE - cpu memset 0 
//  DONE - set pc til 0x100
//  DONE - lag read from file
//  DONE - lag fetch byte
//  lag fetch uint16
//  lag decoder uint8

int decode_opcode(uint8_t opcode)
{

    return 0;

}

int fetch_byte(S_CPU *cpu, S_BUS *bus)
{

    return 0;
}

uint16_t fetch_uint16(S_CPU *cpu, S_BUS *bus)
{

    uint8_t first_byte = bus->memory[cpu->PC++];
    uint16_t second_byte = bus->memory[cpu->PC++];
    int value = (first_byte | (second_byte << 8));
    printf("Fetched values: %08X %08X: %08X\n", first_byte, second_byte, value);
    return value;
}


int fetch(S_CPU *cpu, S_BUS *bus)
{
    while (true)
    {
        // Debug print of state
        printf("PC before fetch: %04X, PC now points at: %02X\n", cpu->PC, bus->memory[cpu->PC]);
        uint8_t byte = bus->memory[cpu->PC];
	cpu->PC = cpu->PC + 1;
        printf("PC after fetch: %04X, PC now points at: %02X\n", cpu->PC, bus->memory[cpu->PC]);
        //decode_opcode(byte);
        switch (byte) {
            case 0x00:
                // NOP
                //fprintf(stderr, "NOP\n");
                break;
	    case 0x21:
		uint16_t value = fetch_uint16(cpu, bus);
		printf("LD HL,n16, fetched %02X\n", value);
		cpu->H = (uint8_t)((value & 0xFF00) >> 8);
		cpu->L = (uint8_t)(value & 0x00FF);
		printf("H: %02X, L: %02X\n", cpu->H, cpu->L);
            case 0xC3:
                cpu->PC = fetch_uint16(cpu, bus);
                printf("Jumped to %04X\n", cpu->PC);
            default:
		printf("Unsupported opcode, skip byte\n");
                // code block
        }

        //Need an exit path
        // if something then exit
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
