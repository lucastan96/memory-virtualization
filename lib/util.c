#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ADDRESS_SPACE_SIZE 65536 // 2 to the power of 16
#define PAGE_SIZE 256

int generate_random_num(int min, int max)
{
    return (rand() % (max + 1 - min)) + min;
}

unsigned char generate_random_char()
{
    const int CHAR_MAX = 126;
    const int CHAR_MIN = 33;

    return (rand() % (CHAR_MAX - CHAR_MIN)) + CHAR_MIN;
}

void populate_address_space(unsigned char *address_space)
{
    int starting_address = generate_random_num(PAGE_SIZE * 2, ADDRESS_SPACE_SIZE);
    int num_of_population = generate_random_num(2048, 20480);
    int ending_address = starting_address + num_of_population;

    if (ending_address > ADDRESS_SPACE_SIZE)
    {
        starting_address = starting_address - (ADDRESS_SPACE_SIZE - ending_address);
    }

    for (int i = starting_address; i < ending_address; ++i)
    {
        address_space[i] = generate_random_char();
    }

    int num_of_frames = (ending_address - starting_address) / PAGE_SIZE;
    int starting_frame = starting_address / PAGE_SIZE;

    for (int i = 0; i <= num_of_frames; ++i)
    {
        address_space[i] = (starting_frame + i);
        address_space[i + PAGE_SIZE] = 1;
    }
}

void write_files(unsigned char *address_space)
{
    FILE *file_physical_memory = fopen("./data/physical_memory.txt", "w");

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
            if (i < PAGE_SIZE * 2)
            {
                fprintf(file_physical_memory, "0x0%05x  |%-3d       |          |%d\n", i, frame, 1);
            }
            else
            {
                if (address_space[i] == 0)
                {
                    fprintf(file_physical_memory, "0x0%05x  |%-3d       |          |%d\n", i, frame, 0);
                }
                else
                {
                    fprintf(file_physical_memory, "0x0%05x  |%-3d       |%c         |%d\n", i, frame, address_space[i], 1);
                }
            }
        }
    }

    fclose(file_physical_memory);

    FILE *file_page_table = fopen("./data/page_table.txt", "w");

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
            if (address_space[i] != 0)
            {
                fprintf(file_page_table, "0x0%02x     |%-3d       |%d\n", i, address_space[i], address_space[i + 256]);
            }
            else
            {
                fprintf(file_page_table, "0x0%02x     |          |%d\n", i, 0);
            }
        }
    }

    fclose(file_page_table);
}

void translate_address(int user_input, unsigned char *address_space)
{
    int vpn = user_input >> 8;
    int pfn = address_space[vpn];

    int offset_mask = 0x00FF;
    int offset = user_input & offset_mask;

    int result = (pfn << 8) + offset;

    printf("%c\n", address_space[result]);
}

void run()
{
    unsigned char *address_space = malloc(ADDRESS_SPACE_SIZE);
    int user_input;

    srand(time(NULL));

    populate_address_space(address_space);
    write_files(address_space);

    while (1)
    {
        scanf("%X", &user_input);
        translate_address(user_input, address_space);
    }

    free(address_space);
}