#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include "parser.h"
#include "vector.h"

int main(int argc, const char *argv[])
{
    const char* fname_in = "test.txt";
    const char* fname_out = "test.out";

    if (argc < 2)
    {
        puts("Please specify a filename and an out filename");
        return 1;
    }
    else
    {
        fname_in = argv[0];
        fname_out = argv[1];
    }

    init_tables();

    FILE *file = fopen(fname_in, "r");
    char line[256];
    vector *bytes = create_vector();

    while (fgets(line, sizeof(line), file))
    {
        // replace first \n by \0
        for (size_t i = 0; line[i]; i++)
            if (line[i] == '\r' || line[i] == '\n')
            {
                line[i] = '\0';
                break;
            }

        parse_line(line, bytes);
    }

    fclose(file);

    FILE *outf = fopen(fname_out, "wb");

    fwrite(bytes->data, 1, bytes->length, outf);

    fclose(outf);

    destroy_vector(bytes);
    destroy_tables();

    return 0;
}