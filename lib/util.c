#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generate_random_num()
{
    srand(time(NULL));
    return rand();
}

void write_file()
{
    FILE *file = fopen("data/physical_memory.txt", "w");

    if (file == NULL)
    {
        printf("Error: Failed to open text file.");
    }
    else
    {
        for (int i = 0; i < 10; ++i)
        {
            fprintf(file, "%d, ", i);
        }
    }

    fclose(file);
}

void run()
{
    const int ADDRESS_SPACE_SIZE = 65536; // 2 to the power of 16
    const int PAGE_SIZE = 256;

    char *address_space = malloc(ADDRESS_SPACE_SIZE);

    int random_num = generate_random_num();
    printf("%d", random_num);

    write_file();
}

// 33 & 127