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

void ld_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    *target_reg = *source_reg;
}

void add_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    if ((*target_reg) + (*source_reg) > 0xFF) {
	cpu->flag_C = 1;
    } else {
	cpu->flag_C = 0;
    }
    *target_reg += *source_reg;
    cpu->flag_Z = (*target_reg == 0);
    cpu->flag_N = 0;
}

void adc_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    uint8_t flag_C = cpu->flag_C;
    if ((*target_reg) + (*source_reg) + flag_C > 0xFF) {
	cpu->flag_C = 1;
    } else {
	cpu->flag_C = 0;
    }
    *target_reg += *source_reg + flag_C;
    cpu->flag_Z = (*target_reg == 0);
    cpu->flag_N = 0;
}

void and_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    *target_reg &= *source_reg;
    cpu->flag_Z = (*target_reg == 0);
    cpu->flag_N = 0;
    cpu->flag_H = 1;
    cpu->flag_C = 0;
}

void xor_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    *target_reg ^= *source_reg;
    cpu->flag_Z = (*target_reg == 0);
    cpu->flag_N = 0;
    cpu->flag_H = 0;
    cpu->flag_C = 0;
}

void or_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    *target_reg |= *source_reg;
    cpu->flag_Z = (*target_reg == 0);
    cpu->flag_N = 0;
    cpu->flag_H = 0;
    cpu->flag_C = 0;
}

void cp_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    if ((*source_reg) > (*target_reg)) {
	cpu->flag_C = 1;
    } else {
	cpu->flag_C = 0;
    }
    cpu->flag_Z = (*target_reg == 0);
    cpu->flag_N = 0;
}

void sub_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    if ((*source_reg) > (*target_reg)) {
	cpu->flag_C = 1;
    } else {
	cpu->flag_C = 0;
    }
    *target_reg -= *source_reg;
    cpu->flag_Z = (*target_reg == 0);
    cpu->flag_N = 0;
}

void sbc_8bit(S_CPU *cpu, uint8_t *target_reg, uint8_t *source_reg) {
    uint8_t flag_C = cpu->flag_C;
    if ((*source_reg) + flag_C > (*target_reg)) {
	cpu->flag_C = 1;
    } else {
	cpu->flag_C = 0;
    }
    *target_reg -= *source_reg - flag_C;
    cpu->flag_Z = (*target_reg == 0);
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
            case 0xC3:
                cpu->PC = fetch_uint16(cpu, bus);
                printf("Jumped to %04X\n", cpu->PC);
		break;

	    // RST
            case 0xC7:
                cpu->PC = 0x00;
                printf("Reset to %04X\n", cpu->PC);
		break;
            case 0xD7:
                cpu->PC = 0x10;
                printf("Reset to %04X\n", cpu->PC);
		break;
            case 0xE7:
                cpu->PC = 0x20;
                printf("Reset to %04X\n", cpu->PC);
		break;
            case 0xF7:
                cpu->PC = 0x30;
                printf("Reset to %04X\n", cpu->PC);
		break;
            case 0xCF:
                cpu->PC = 0x08;
                printf("Reset to %04X\n", cpu->PC);
		break;
            case 0xDF:
                cpu->PC = 0x18;
                printf("Reset to %04X\n", cpu->PC);
		break;
            case 0xEF:
                cpu->PC = 0x28;
                printf("Reset to %04X\n", cpu->PC);
		break;
            case 0xFF:
                cpu->PC = 0x38;
                printf("Reset to %04X\n", cpu->PC);
		break;

            // LD
            case 0x40:
                ld_8bit(cpu, &cpu->B, &cpu->B);
                break;
            case 0x41:
                ld_8bit(cpu, &cpu->B, &cpu->C);
                break;
            case 0x42:
                ld_8bit(cpu, &cpu->B, &cpu->D);
                break;
            case 0x43:
                ld_8bit(cpu, &cpu->B, &cpu->E);
                break;
            case 0x44:
                ld_8bit(cpu, &cpu->B, &cpu->H);
                break;
            case 0x45:
                ld_8bit(cpu, &cpu->B, &cpu->L);
                break;
            case 0x47:
                ld_8bit(cpu, &cpu->B, &cpu->A);
                break;
            case 0x48:
                ld_8bit(cpu, &cpu->C, &cpu->B);
                break;
            case 0x49:
                ld_8bit(cpu, &cpu->C, &cpu->C);
                break;
            case 0x4A:
                ld_8bit(cpu, &cpu->C, &cpu->D);
                break;
            case 0x4B:
                ld_8bit(cpu, &cpu->C, &cpu->E);
                break;
            case 0x4C:
                ld_8bit(cpu, &cpu->C, &cpu->H);
                break;
            case 0x4D:
                ld_8bit(cpu, &cpu->C, &cpu->L);
                break;
            case 0x4F:
                ld_8bit(cpu, &cpu->C, &cpu->A);
                break;

            case 0x50:
                ld_8bit(cpu, &cpu->D, &cpu->B);
                break;
            case 0x51:
                ld_8bit(cpu, &cpu->D, &cpu->C);
                break;
            case 0x52:
                ld_8bit(cpu, &cpu->D, &cpu->D);
                break;
            case 0x53:
                ld_8bit(cpu, &cpu->D, &cpu->E);
                break;
            case 0x54:
                ld_8bit(cpu, &cpu->D, &cpu->H);
                break;
            case 0x55:
                ld_8bit(cpu, &cpu->D, &cpu->L);
                break;
            case 0x57:
                ld_8bit(cpu, &cpu->D, &cpu->A);
                break;
            case 0x58:
                ld_8bit(cpu, &cpu->E, &cpu->B);
                break;
            case 0x59:
                ld_8bit(cpu, &cpu->E, &cpu->C);
                break;
            case 0x5A:
                ld_8bit(cpu, &cpu->E, &cpu->D);
                break;
            case 0x5B:
                ld_8bit(cpu, &cpu->E, &cpu->E);
                break;
            case 0x5C:
                ld_8bit(cpu, &cpu->E, &cpu->H);
                break;
            case 0x5D:
                ld_8bit(cpu, &cpu->E, &cpu->L);
                break;
            case 0x5F:
                ld_8bit(cpu, &cpu->E, &cpu->A);
                break;

            case 0x60:
                ld_8bit(cpu, &cpu->H, &cpu->B);
                break;
            case 0x61:
                ld_8bit(cpu, &cpu->H, &cpu->C);
                break;
            case 0x62:
                ld_8bit(cpu, &cpu->H, &cpu->D);
                break;
            case 0x63:
                ld_8bit(cpu, &cpu->H, &cpu->E);
                break;
            case 0x64:
                ld_8bit(cpu, &cpu->H, &cpu->H);
                break;
            case 0x65:
                ld_8bit(cpu, &cpu->H, &cpu->L);
                break;
            case 0x67:
                ld_8bit(cpu, &cpu->H, &cpu->A);
                break;
            case 0x68:
                ld_8bit(cpu, &cpu->L, &cpu->B);
                break;
            case 0x69:
                ld_8bit(cpu, &cpu->L, &cpu->C);
                break;
            case 0x6A:
                ld_8bit(cpu, &cpu->L, &cpu->D);
                break;
            case 0x6B:
                ld_8bit(cpu, &cpu->L, &cpu->E);
                break;
            case 0x6C:
                ld_8bit(cpu, &cpu->L, &cpu->H);
                break;
            case 0x6D:
                ld_8bit(cpu, &cpu->L, &cpu->L);
                break;
            case 0x6F:
                ld_8bit(cpu, &cpu->L, &cpu->A);
                break;

            case 0x78:
                ld_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0x79:
                ld_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0x7A:
                ld_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0x7B:
                ld_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0x7C:
                ld_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0x7D:
                ld_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0x7F:
                ld_8bit(cpu, &cpu->A, &cpu->A);
                break;

            // ADD
            case 0x80:
                add_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0x81:
                add_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0x82:
                add_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0x83:
                add_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0x84:
                add_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0x85:
                add_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0x87:
                add_8bit(cpu, &cpu->A, &cpu->A);
                break;

            case 0x88:
                adc_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0x89:
                adc_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0x8A:
                adc_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0x8B:
                adc_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0x8C:
                adc_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0x8D:
                adc_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0x8F:
                adc_8bit(cpu, &cpu->A, &cpu->A);
                break;

	    // SUB
            case 0x90:
                sub_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0x91:
                sub_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0x92:
                sub_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0x93:
                sub_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0x94:
                sub_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0x95:
                sub_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0x97:
                sub_8bit(cpu, &cpu->A, &cpu->A);
                break;

            case 0x98:
                sbc_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0x99:
                sbc_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0x9A:
                sbc_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0x9B:
                sbc_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0x9C:
                sbc_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0x9D:
                sbc_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0x9F:
                sbc_8bit(cpu, &cpu->A, &cpu->A);
                break;

	    // AND
            case 0xA0:
                and_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0xA1:
                and_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0xA2:
                and_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0xA3:
                and_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0xA4:
                and_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0xA5:
                and_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0xA7:
                and_8bit(cpu, &cpu->A, &cpu->A);
                break;

            case 0xA8:
                xor_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0xA9:
                xor_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0xAA:
                xor_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0xAB:
                xor_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0xAC:
                xor_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0xAD:
                xor_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0xAF:
                xor_8bit(cpu, &cpu->A, &cpu->A);
                break;

            case 0xB0:
                or_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0xB1:
                or_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0xB2:
                or_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0xB3:
                or_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0xB4:
                or_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0xB5:
                or_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0xB7:
                or_8bit(cpu, &cpu->A, &cpu->A);
                break;

            case 0xB8:
                cp_8bit(cpu, &cpu->A, &cpu->B);
                break;
            case 0xB9:
                cp_8bit(cpu, &cpu->A, &cpu->C);
                break;
            case 0xBA:
                cp_8bit(cpu, &cpu->A, &cpu->D);
                break;
            case 0xBB:
                cp_8bit(cpu, &cpu->A, &cpu->E);
                break;
            case 0xBC:
                cp_8bit(cpu, &cpu->A, &cpu->H);
                break;
            case 0xBD:
                cp_8bit(cpu, &cpu->A, &cpu->L);
                break;
            case 0xBF:
                cp_8bit(cpu, &cpu->A, &cpu->A);
                break;

	    // Subroutines
	    case 0x31:
		cpu->SP = fetch_uint16(cpu, bus);
		cpu->SP++;
		break;
	    case 0xC0:
		if (!cpu->flag_Z) {
		    cpu->PC = --(cpu->SP);
		}
		break;
	    case 0xD0:
		if (!cpu->flag_C) {
		    cpu->PC = --(cpu->SP);
		}
		break;
	    case 0xC8:
		if (cpu->flag_Z) {
		    cpu->PC = --(cpu->SP);
		}
		break;
	    case 0xD8:
		if (cpu->flag_C) {
		    cpu->PC = --(cpu->SP);
		}
		break;
	    case 0xC9:
		cpu->PC = --(cpu->SP);
		break;
	    case 0xCD:
		bus->memory[cpu->SP++] = cpu->PC;
		cpu->PC = fetch_uint16(cpu, bus);
		break;
	    case 0xC4:
		if (!cpu->flag_Z) {
		    bus->memory[cpu->SP++] = cpu->PC;
		    cpu->PC = fetch_uint16(cpu, bus);
		}
		break;
	    case 0xD4:
		if (!cpu->flag_C) {
		    bus->memory[cpu->SP++] = cpu->PC;
		    cpu->PC = fetch_uint16(cpu, bus);
		}
		break;
	    case 0xCC:
		if (cpu->flag_Z) {
		    bus->memory[cpu->SP++] = cpu->PC;
		    cpu->PC = fetch_uint16(cpu, bus);
		}
		break;
	    case 0xDC:
		if (cpu->flag_C) {
		    bus->memory[cpu->SP++] = cpu->PC;
		    cpu->PC = fetch_uint16(cpu, bus);
		}
		break;
	    case 0xFB:
		printf("EI\n");
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
