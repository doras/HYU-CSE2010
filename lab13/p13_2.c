#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *input = fopen("input.txt", "r");

    fclose(input);
    return 0;
}