#include <stdio.h>
#include <stdlib.h>
#include "lib/util.h"

int main()
{
    const int ADDRESS_SPACE_SIZE = 65536; // 2 to the power of 16
    const int PAGE_SIZE = 256;

    char *address_space = malloc(ADDRESS_SPACE_SIZE);

    write_file();

    return 0;
}