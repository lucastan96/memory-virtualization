#include <stdio.h>

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

// 33 & 127