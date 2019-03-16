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
    int starting_address = generate_random_num(PAGE_SIZE * 2, ADDRESS_SPACE_SIZE);
    int num_of_population = generate_random_num(2048, 20480);

    if (starting_address + num_of_population > ADDRESS_SPACE_SIZE)
    {
        starting_address = starting_address - (ADDRESS_SPACE_SIZE - starting_address + num_of_population);
    }

    for (int i = starting_address; i < starting_address + num_of_population; ++i)
    {
        address_space[i] = generate_random_char();
    }
}

void write_files(char *address_space)
{
    FILE *file_physical_memory = fopen("./data/physical_memory.txt", "w");

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
            frame = i / PAGE_SIZE;
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
                ending_frame = frame;
            }
        }
    }

    fclose(file_physical_memory);

    FILE *file_page_table = fopen("./data/page_table.txt", "w");

    printf("start: %d\n", starting_frame);
    printf("end: %d\n", ending_frame);

    int present_bit = 1;

    if (file_page_table == NULL)
    {
        printf("Error: Failed to open page table text file.");
    }
    else
    {
        fprintf(file_page_table, "Page      |Frame     |Present Bit\n");
        fprintf(file_page_table, "----------|----------|------------\n");
        for (int i = 0; i < PAGE_SIZE; ++i)
        {
            if (i <= ending_frame - starting_frame)
            {
                fprintf(file_page_table, "0x0%02x     |%-3d       |%d\n", i, starting_frame + i, present_bit);
            }
            else
            {
                fprintf(file_page_table, "0x0%02x     |          |%d\n", i, present_bit);
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