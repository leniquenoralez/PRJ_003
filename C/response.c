#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/stat.h>
#include "http_common.h"
#include <dirent.h>
typedef struct TableRow
{
    char file_type[256];
    char name[356];
    char last_modified[256];
    int size;
} tableRow;

int IsIndexUri(char *path)
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
    sprintf(response->headers.base.entity.content_length, "Content-Length: %ld\r\n", file_size);
    strcat(response->message, response->headers.base.entity.content_length);
    struct stat attr;
    stat(path, &attr);
    sprintf(response->headers.base.entity.content_length, "Last-Modified: %s\r\n", ctime(&attr.st_mtime));
    strcat(response->message, response->headers.base.entity.content_length);

    char *html_buffer = (char *)malloc(file_size);
    fread(html_buffer, 1, file_size, index_html);
    strcat(response->message, "\r\n");
    strcat(response->message, html_buffer);
    response->message_length = strlen(response->message);
    return 0;
}
int WriteResponseHeaders(httpResponseHeaders *response_headers)
{
    memset(response_headers->Server, '\0', BUFFER_SIZE);
    memset(response_headers->base.entity.content_type, '\0', BUFFER_SIZE);

    time_t curr_time;
    time(&curr_time);

    sprintf(response_headers->base.general.date, "Date: %s", ctime(&curr_time));
    strcpy(response_headers->Server, "Server: SWS\r\n");
    strcpy(response_headers->base.entity.content_type, "Content-Type: text/html\r\n");
    return 0;
}
void getType(const struct stat sb, char *file_type)
{
    if (S_ISREG(sb.st_mode))
    {
        strcpy(file_type, "TXT");
    }
    else if (S_ISDIR(sb.st_mode))
    {
        strcpy(file_type, "DIR");
    }
    else
    {
        strcpy(file_type, "-");
    }
}

int GetFileInfo(char *path, tableRow *row)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == -1)
    {
        fprintf(stderr, "Can't stat %s\n", path);
        return -1;
    }

    memset(row->file_type, '\0', 13);
    getType(statbuf, row->file_type);
    row->size = statbuf.st_size;
    strcpy(row->last_modified, ctime(&statbuf.st_mtime));
    if (strcmp(row->file_type, "-") == 0)
    {
        return -1;
    }
    return 0;
}
void WriteTableRow(char *path, char *filename, char *tr)
{
    strcat(tr, "<tr>");
    tableRow row;
    memset(row.name, '\0', 256);
    char *fullPath = (char *)malloc(strlen(path) + strlen(filename) + 2);
    strcpy(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, filename);
    GetFileInfo(fullPath, &row);
    if (strcmp(row.file_type, "DIR") == 0)
    {
        strcat(row.name, "/");
        strcat(row.name, filename);
    }
    else
    {
        strcpy(row.name, filename);
    }

    char *temp = (char *)malloc(256);
    sprintf(temp, "<td>[%s]</td>", row.file_type);
    strcat(tr, temp);
    sprintf(temp, "<td><a href=\"%s\">%s</a></td>", row.name, row.name);
    strcat(tr, temp);
    sprintf(temp, "<td align=\"right\">%s</td>", row.last_modified);
    strcat(tr, temp);
    sprintf(temp, "<td align=\"right\">%s</td>", row.file_type);
    strcat(tr, temp);
    strcat(tr, "</tr>");
}

int GenerateTable(char*table, char *tableBody){
    FILE *inputFile = fopen("index.template.html", "r");
    if (inputFile == NULL)
    {
        perror("Error opening input file");
        return 1;
    }
    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    rewind(inputFile);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (table == NULL)
    {
        perror("Memory allocation error");
        fclose(inputFile);
        return 1;
    }

    fread(fileContent, 1, fileSize, inputFile);
    fileContent[fileSize] = '\0';
    fclose(inputFile);

    char *found = strstr(fileContent, "%{content}%");
    if (found != NULL)
    {
        size_t searchLen = strlen("%{content}%");

        strncpy(table, fileContent, (found - fileContent));
        strcat(table, tableBody);
        strcat(table, found + searchLen);
        table[strlen(table)] = '\0';
    }
    return 0;
}

int GenerateIndexHtml(char *path, httpResponse *response)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(path)) == NULL)
    {
        return -1;
    }
    char tableBody[BUFFER_SIZE];
    memset(tableBody, '\0', BUFFER_SIZE);
    while ((dirp = readdir(dp)) != NULL)
    {
        if (dirp->d_name[0] != '.')
        {
            char row[256];
            memset(row, '\0', 256);
            WriteTableRow(path, dirp->d_name, row);
            strcat(tableBody, row);
        }
        
    }
    char table[BUFFER_SIZE];
    memset(table, '\0', BUFFER_SIZE);
    GenerateTable(table, tableBody);
    time_t curr_time;
    time(&curr_time);
    sprintf(response->headers.base.entity.content_length, "Content-Length: %ld\r\n", strlen(table));
    strcat(response->message, response->headers.base.entity.content_length);
    sprintf(response->headers.base.entity.content_length, "Last-Modified: %s\r\n", ctime(&curr_time));
    strcat(response->message, response->headers.base.entity.content_length);

    strcat(response->message, "\r\n");
    strcat(response->message, table);
    response->message_length = strlen(response->message);
    return 0;
}

int WriteResponse(httpRequestLine *request_line, httpRequestHeaders *request_headers, httpResponse *response)
{
    if (IsDir(request_line->uri) == 0 && HasIndexHtml(request_line->uri) == 0)
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

    WriteResponseHeaders(&response->headers);
    strcat(response->message, response->headers.base.general.date);
    strcat(response->message, response->headers.Server);
    strcat(response->message, response->headers.base.entity.content_type);

    if (IsIndexUri(request_line->uri) == 0)
    {
        WriteIndexHtml(request_line->uri, response);
    }
    if (IsDir(request_line->uri) == 0 && HasIndexHtml(request_line->uri) == -1)
    {
        GenerateIndexHtml(request_line->uri, response);
    }

    return 0;
}