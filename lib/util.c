#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ADDRESS_SPACE_SIZE 65536 // 2 to the power of 16
#define PAGE_SIZE 256

int generate_random_num(int min, int max)
{
    return (rand() % (max - min)) + min;
}

char generate_random_char()
{
    const int CHAR_MAX = 126;
    const int CHAR_MIN = 33;

    return (rand() % (CHAR_MAX - CHAR_MIN)) + CHAR_MIN;
}

void populate_address_space(char *address_space)
{
    for (int i = 512; i < 512 + generate_random_num(2048, 20480); ++i)
    {
        address_space[i] = generate_random_char();
    }
}

void write_file(char *address_space)
{
    FILE *file = fopen("data/physical_memory.txt", "w");

    if (file == NULL)
    {
        printf("Error: Failed to open text file.");
    }
    else
    {
        fprintf(file, "Address   |Frame     |Content   \n");
        fprintf(file, "----------|----------|----------\n");
        for (int i = 0; i < 10; ++i)
        {
            fprintf(file, "%d         |1         |a       \n", i);
        }
    }

    fclose(file);
}

void run()
{
    srand(time(NULL));
    char *address_space = malloc(ADDRESS_SPACE_SIZE);
    populate_address_space(address_space);
    write_file(address_space);
}