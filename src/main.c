#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu.h"

int main(int argc, char *argv[]) {
    // Check if the user provided a filename
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Open the binary file for reading
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == -1) {
        perror("Error getting file size");
        fclose(file);
        return 1;
    }
    rewind(file);

    // Allocate memory for the uint8_t array
    uint8_t *buffer = (uint8_t *)malloc(file_size * sizeof(uint8_t));
    if (buffer == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return 1;
    }

    // Read the binary data into the buffer
    size_t read_size = fread(buffer, sizeof(uint8_t), file_size, file);
    if (read_size != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return 1;
    }

    // Print the buffer content in hex
    printf("Hex dump of the file:\n");
    for (long i = 0; i < file_size; i++) {
        printf("%02X ", buffer[i]);  // Print each byte in hex format
        if ((i + 1) % 16 == 0)       // New line every 16 bytes
            printf("\n");
    }
    printf("\n");

    // Clean up
    free(buffer);
    fclose(file);

    cpu_init(buffer, file_size);
    return 0;
}
