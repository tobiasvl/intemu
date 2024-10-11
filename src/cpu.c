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

uint8_t fetch_byte(S_CPU *cpu, S_BUS *bus)
{

    return bus->memory[cpu->PC++];
}

uint16_t fetch_uint16(S_CPU *cpu, S_BUS *bus)
{

    uint8_t first_byte = fetch_byte(cpu, bus);
    uint16_t second_byte = fetch_byte(cpu, bus);
    int value = (first_byte | (second_byte << 8));
    printf("Fetched values: %08X %08X: %08X\n", first_byte, second_byte, value);
    return value;
}

void increment_register(S_CPU *cpu, uint8_t *reg) {
    (*reg)++;
    cpu->flag_Z = (*reg == 0);
    cpu->flag_N = 0;
}

void decrement_register(S_CPU *cpu, uint8_t *reg) {
    (*reg)--;
    cpu->flag_Z = (*reg == 0);
    cpu->flag_N = 0;
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
            case 0x04:
		increment_register(cpu, &cpu->B);
                break;
            case 0x05:
		decrement_register(cpu, &cpu->B);
                break;
            case 0x0C:
		increment_register(cpu, &cpu->C);
                break;
            case 0x0D:
		decrement_register(cpu, &cpu->C);
                break;
	    case 0x0E:
		cpu->C = fetch_byte(cpu, bus);
		break;
	    case 0x11:
		uint16_t value = fetch_uint16(cpu, bus);
		printf("LD DE,n16, fetched %02X\n", value);
		cpu->D = (uint8_t)((value & 0xFF00) >> 8);
		cpu->E = (uint8_t)(value & 0x00FF);
		printf("D: %02X, E: %02X\n", cpu->D, cpu->E);
		break;
	    case 0x12:
		printf("LD [DE],A\n");
    		int address = ((uint16_t)(cpu->D) << 8) | cpu->E;
		printf("Address in DE is %04X, value is %02X\n", address, bus->memory[address]);
		bus->memory[address++] = cpu->A;
		printf("A is %02X\n", cpu->A);
		break;
	    case 0x14:
		increment_register(cpu, &cpu->D);
                break;
            case 0x15:
		decrement_register(cpu, &cpu->D);
                break;
            case 0x1C:
		increment_register(cpu, &cpu->E);
                break;
            case 0x1D:
		decrement_register(cpu, &cpu->E);
                break;
            case 0x20:
                printf("JR NZ\n");
		if (!cpu->flag_Z) {
                    int8_t relative_address = (int8_t)fetch_byte(cpu, bus);
                    uint16_t new_address = cpu->PC + relative_address;
                    printf("Z is false, jump %d memory addresses to new_address %04X\n", relative_address, new_address);
                    cpu->PC = new_address;
                } else {
		    printf("Z is true, don't jump\n");
                }
                break;

	    case 0x21:
		uint16_t value2 = fetch_uint16(cpu, bus);
		printf("LD HL,n16, fetched %02X\n", value2);
		cpu->H = (uint8_t)((value2 & 0xFF00) >> 8);
		cpu->L = (uint8_t)(value2 & 0x00FF);
		printf("H: %02X, L: %02X\n", cpu->H, cpu->L);
		break;
            case 0x24:
		increment_register(cpu, &cpu->H);
                break;
            case 0x25:
		decrement_register(cpu, &cpu->H);
                break;
	    case 0x2A:
		printf("LD A, [HL+]\n");
    		int address2 = ((uint16_t)(cpu->H) << 8) | cpu->L;
		printf("Address in HL is %04X, value is %02X\n", address2, bus->memory[address2]);
		cpu->A = bus->memory[address2++];
		cpu->H = address2 >> 8;
		cpu->L = address2 & 0x00FF;
		printf("Address in HL is now %04X, A is %02X\n", address2, cpu->A);
		break;
            case 0x2C:
		increment_register(cpu, &cpu->L);
                break;
            case 0x2D:
		decrement_register(cpu, &cpu->L);
                break;
            case 0x3C:
		increment_register(cpu, &cpu->A);
                break;
            case 0x3D:
		decrement_register(cpu, &cpu->A);
                break;
	    case 0x47:
		cpu->B = cpu->A;
		break;
            case 0xC3:
                cpu->PC = fetch_uint16(cpu, bus);
                printf("Jumped to %04X\n", cpu->PC);
		break;
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
