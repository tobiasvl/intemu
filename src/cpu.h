#include <stdint.h>

typedef struct cpu
{
    uint8_t A;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t H;
    uint8_t L;
    uint8_t F;
    uint16_t PC;
    uint16_t SP;
}S_CPU;

int cpu_init(uint8_t *file_buffer, ssize_t file_size);
