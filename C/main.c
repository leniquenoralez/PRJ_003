#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

char *DIRECTORY = NULL;
char *ADDRESS = NULL;
char *LOG_FILE = NULL;
int PORT = -1;
int decodeFlags(int argc, char **argv)
{
    int opt;

    opterr = 0;
    while ((opt = getopt(argc, argv, ":dhc:i:l:p:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            DIRECTORY = optarg;
            break;
        case 'i':
            ADDRESS = optarg;
            break;
        case 'l':
            LOG_FILE = optarg;
            break;
        case 'p':
            PORT = atoi(optarg);
            break;
        case 'd':
        case 'h':
        case '?':
            fprintf(stderr, "Unknown option: -%c\n", optopt);
            return 1;
        case ':':
            fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            return 1;
        default:
            printf("==> %c\n", opt);
            break;
        }
    }
    return 0;
}
int main(int argc, char **argv)
{
    decodeFlags(argc, argv);
    if (DIRECTORY != NULL)
    {
        printf("DIRECTORY: %s\n", DIRECTORY);
    }

    if (ADDRESS != NULL)
    {
        printf("ADDRESS: %s\n", ADDRESS);
    }
    if (PORT != -1)
    {
        printf("PORT: %d\n", PORT);
    }
    if (LOG_FILE != NULL)
    {
        printf("LOG_FILE: %s\n", LOG_FILE);
    }
    return 0;
}

