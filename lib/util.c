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

void write_files(char *address_space)
{
    FILE *file_physical_memory = fopen("./data/physical_memory.txt", "w");
    FILE *file_page_table = fopen("./data/page_table.txt", "w");

    int starting_frame = 0;
    int ending_frame = 0;

    if (file_physical_memory == NULL)
    {
        printf("Error: Failed to open physical memory text file.");
    }
    else
    {
        int frame = 0;
        fprintf(file_physical_memory, "Address   |Frame     |Content   |Used\n");
        fprintf(file_physical_memory, "----------|----------|----------|----------\n");
        for (int i = 0; i < ADDRESS_SPACE_SIZE; ++i)
        {
            frame = i / 256;
            if (address_space[i] == 0)
            {
                fprintf(file_physical_memory, "0x0%05x  |%-3d       |          |%d\n", i, frame, 0);
            }
            else
            {
                fprintf(file_physical_memory, "0x0%05x  |%-3d       |%c         |%d\n", i, frame, address_space[i], 1);
                if (starting_frame == 0)
                {
                    starting_frame = frame;
                }
                ending_frame = frame - 1;
            }
        }
    }

    fclose(file_physical_memory);

    if (file_page_table == NULL)
    {
        printf("Error: Failed to open page table text file.");
    }
    else
    {
        fprintf(file_page_table, "Page      |Frame     \n");
        fprintf(file_page_table, "----------|----------\n");
        for (int i = 0; i < PAGE_SIZE; ++i)
        {
            if (i < ending_frame)
            {
                fprintf(file_page_table, "0x0%02x     |%d\n", i, starting_frame + i);
            }
            else
            {
                fprintf(file_page_table, "0x0%02x     |\n", i);
            }
        }
    }
}

void run()
{
    srand(time(NULL));
    char *address_space = malloc(ADDRESS_SPACE_SIZE);
    populate_address_space(address_space);
    write_files(address_space);
}