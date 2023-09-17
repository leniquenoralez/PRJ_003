#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "http_common.h"
    int
    IsIndexUri(char *path)
{
    const char *suffix = "index.html";
    size_t path_len = strlen(path);
    size_t suffix_len = strlen(suffix);
    if (path_len >= suffix_len && strcmp(path + path_len - suffix_len, suffix) == 0)
    {
        return 0;
    }
    return -1;
}

int IsDir(char *path)
{

    struct stat path_stat;

    if (stat(path, &path_stat) != 0)
    {
        perror("stat() error");
        return 1;
    }

    if (S_ISDIR(path_stat.st_mode))
    {
        return 0;
    }
    return -1;
}
int HasIndexHtml(char *path)
{
    if (IsDir(path) == -1)
    {
        return -1;
    }

    DIR *directory = opendir(path);
    if (directory == NULL)
    {
        perror("opendir() error");
        return 1;
    }

    int found = -1;

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, "index.html") == 0)
        {
            found = 0;
            break;
        }
    }
    closedir(directory);
    return found;
}

int WriteIndexHtml(char *path, httpResponse *response)
{
    FILE *index_html = fopen(path, "rb");

    if (index_html == NULL)
    {
        return -1;
    }

    fseek(index_html, 0, SEEK_END);

    long file_size = ftell(index_html);
    rewind(index_html);

    char *html_buffer = (char *)malloc(file_size);
    fread(html_buffer, 1, file_size, index_html);
    strncpy(response->message, html_buffer, file_size);
    response->message_length = file_size;
    return 0;
}

int WriteResponse(httpRequestLine *request_line, httpRequestHeaders *request_headers, httpResponse *response)
{
    // TODO: write cgi results if uri starts with  /cgi-bin
    if (IsDir(request_line->uri) == 0 && HasIndexHtml(request_line->uri))
    {
        const char *suffix = "/index.html";
        char *combinedPath = (char *)malloc(strlen(request_line->uri) + strlen(suffix) + 2); // +2 for the slash and null terminator
        if (combinedPath == NULL)
        {
            perror("malloc() error");
            return -1;
        }

        strcpy(combinedPath, request_line->uri);
        strcat(combinedPath, suffix);
        strncpy(request_line->uri, combinedPath, strlen(combinedPath) + 1);
    }
    memset(response->message, '\0', BUFFER_SIZE);

    // TODO: generate response header
    // TODO: add headers to response message
    if (IsIndexUri(request_line->uri) == 0)
    {
        WriteIndexHtml(request_line->uri, response);
    }

    // TODO: if index.html does not exist then create one with the directory contents as the html contents

    return 0;
}