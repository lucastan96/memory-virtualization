#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PAGE_SIZE 256
#define ADDRESS_SPACE_SIZE 65536 // 2 to the power of 16
#define DISK_SPACE_SIZE 512

int generate_random_num(int min, int max)
{
    return (rand() % (max + 1 - min)) + min;
}

unsigned char generate_random_char()
{
    const int CHAR_MAX = 125;
    const int CHAR_MIN = 33;

    return (rand() % (CHAR_MAX - CHAR_MIN)) + CHAR_MIN;
}

void populate_address_space(unsigned char *address_space)
{
    for (int i = 0; i < ADDRESS_SPACE_SIZE; ++i)
    {
        address_space[i] = '~';
    }

    int starting_address = generate_random_num(PAGE_SIZE * 2, 2048);
    int num_of_population = generate_random_num(2048, 20480);
    int ending_address = starting_address + num_of_population;

    for (int i = starting_address; i < ending_address; ++i)
    {
        address_space[i] = generate_random_char();
    }

    int starting_frame = starting_address / PAGE_SIZE;
    int ending_frame = ending_address / PAGE_SIZE;
    int num_of_frames = ending_frame - starting_frame;

    for (int i = 0; i <= num_of_frames; ++i)
    {
        address_space[i] = (starting_frame + i);
        address_space[i + PAGE_SIZE] = 1;
    }
}

void populate_disk_space(unsigned char *disk_space, unsigned char *address_space)
{
    for (int i = 0; i < DISK_SPACE_SIZE; ++i)
    {
        disk_space[i] = generate_random_char();
    }

    int num_of_frames = DISK_SPACE_SIZE / PAGE_SIZE;
    int current_frame = 0;
    for (int i = 0; i < PAGE_SIZE; ++i)
    {
        if (address_space[i] == '~' && current_frame < num_of_frames)
        {
            address_space[i] = current_frame;
            address_space[i + PAGE_SIZE] = 0;
            ++current_frame;
        }
    }
}

void write_files(unsigned char *address_space, unsigned char *disk_space)
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
                if (address_space[i] == '~')
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

    FILE *file_disk_space = fopen("./data/disk_space.txt", "w");

    if (file_disk_space == NULL)
    {
        printf("Error: Failed to open disk space text file.");
    }
    else
    {
        int frame = 0;
        fprintf(file_disk_space, "Address   |Frame     |Content   |Used\n");
        fprintf(file_disk_space, "----------|----------|----------|----------\n");
        for (int i = 0; i < DISK_SPACE_SIZE; ++i)
        {
            frame = i / PAGE_SIZE;
            if (disk_space[i] == '~')
            {
                fprintf(file_disk_space, "0x0%05x  |%-3d       |          |%d\n", i, frame, 0);
            }
            else
            {
                fprintf(file_disk_space, "0x0%05x  |%-3d       |%c         |%d\n", i, frame, disk_space[i], 1);
            }
        }
    }

    fclose(file_disk_space);

    printf("\n----------------------------------------------------\n");
    printf("Virtual addresses in disk space:");

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
            if (address_space[i] == '~')
            {
                fprintf(file_page_table, "0x0%02x     |          |\n", i);
            }
            else
            {
                fprintf(file_page_table, "0x0%02x     |%-3d       |%d\n", i, address_space[i], address_space[i + PAGE_SIZE]);
                if (address_space[i + PAGE_SIZE] == 0)
                {
                    printf(" 0x0%02x", i);
                }
            }
        }
    }

    fclose(file_page_table);

    printf("\n");
}

void print_console(int user_input, int vpn, int offset, int pfn, int pfn_plus_offset, unsigned char *address_space, int is_swap)
{
    printf("Address: 0x%X\n", user_input);
    printf("VPN: 0x%X\n", vpn);
    printf("Offset: 0x%X\n", offset);
    printf("PFN: 0x%02X\n", pfn);
    if (is_swap == 1)
    {
        printf("New PFN + Offset: 0x0%05X", pfn_plus_offset);
    }
    else
    {
        printf("PFN + Offset: 0x0%05X", pfn_plus_offset);
    }
    printf("\nContent: %c\n", address_space[pfn_plus_offset]);
}

void translate_address(int user_input, unsigned char *address_space, unsigned char *disk_space)
{
    int vpn = user_input >> 8;
    int pfn = address_space[vpn];

    int offset_mask = 0x00FF;
    int offset = user_input & offset_mask;

    int pfn_plus_offset = (pfn << 8) + offset;

    printf("----------------------------------------------------\n");

    if (address_space[vpn + PAGE_SIZE] == 0)
    {
        int free_frame = 0;
        for (int i = 0; i < PAGE_SIZE; ++i)
        {
            if (address_space[i] == '~')
            {
                free_frame = (address_space[i - 1] + 1);
            }
        }

        int disk_space_pos = 0;
        for (int i = free_frame * 256; i < (free_frame * 256) + 256; ++i)
        {
            address_space[i] = disk_space[disk_space_pos];
            ++disk_space_pos;
        }

        address_space[vpn] = free_frame;
        address_space[vpn + 256] = 1;
        pfn_plus_offset = (free_frame << 8) + offset;

        printf("Page fault exception encountered.\n");
        printf("Swapped content from disk to physical memory.\n");
        printf("----------------------------------------------------\n");

        print_console(user_input, vpn, offset, pfn, pfn_plus_offset, address_space, 1);
        write_files(address_space, disk_space);
    }
    else if (address_space[pfn_plus_offset] == '~')
    {
        printf("Address not used.\n");
    }
    else
    {
        print_console(user_input, vpn, offset, pfn, pfn_plus_offset, address_space, 0);
    }
}

void print_description()
{
    printf("");
}

void run()
{
    unsigned char *address_space = malloc(ADDRESS_SPACE_SIZE);
    unsigned char *disk_space = malloc(DISK_SPACE_SIZE);
    int user_input;

    srand(time(NULL));

    populate_address_space(address_space);
    populate_disk_space(disk_space, address_space);
    write_files(address_space, disk_space);
    print_description();

    while (1)
    {
        printf("----------------------------------------------------\n");
        printf("Please enter a virtual memory address (0x____): ");
        scanf("%X", &user_input);
        translate_address(user_input, address_space, disk_space);
    }

    free(address_space);
    free(disk_space);
}