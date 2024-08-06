#include <stdio.h>
#include <stdint.h>

// Create a new type called BYTE to be a uint8_t
typedef uint8_t BYTE;

// Define the block size
#define BLOCK_SIZE 512

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // Open file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 1;
    }

    BYTE buffer[BLOCK_SIZE];
    FILE *current_file = NULL;
    char filename[8];
    int counter = 0;

    // Repeat until end of card
    while (fread(buffer, BLOCK_SIZE, 1, file))
    {
        // If start of a new JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // If already found a JPEG, close it
            if (current_file != NULL)
            {
                fclose(current_file);
            }

            // Name a new JPEG
            sprintf(filename, "%03i.jpg", counter++);

            // Open a new JPEG file for writing
            current_file = fopen(filename, "w");
            if (current_file == NULL)
            {
                return 1;
            }
        }

        // If already found a JPEG, write to it
        if (current_file != NULL)
        {
            fwrite(buffer, BLOCK_SIZE, 1, current_file);
        }
    }

    // Close any remaining files
    if (current_file != NULL)
    {
        fclose(current_file);
    }

    // Close raw file
    fclose(file);

    return 0;
}
