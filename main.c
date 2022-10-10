#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef WINDOWS
#include <direct.h>
#define cd _getcwd
#else
#include <unistd.h>
#define cd getcwd
#endif

uint64_t bufSize = 32768;
const char validChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
                           'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd', 'e', 'f'};

int exists(const char *);
char *loadFile(const char *);
char *processBuffer(char *buf);
unsigned char *convert(const char *s, int *length);
static unsigned char gethex(const char *s, char **endptr);
void writeFile(const char *fileName, char *buf);

int exists(const char *file)
{
    if (access(file, F_OK) == 0)
    {
        return 1;
    }
    return 0;
}

char *loadFile(const char *file)
{
    char *buf = (char *)calloc(bufSize, sizeof(char));
    FILE *fp = fopen(file, "r");
    char c;
    int counter = 0;
    if (fp == NULL)
    {
        printf("error could not open file");
        exit(1);
    }
    do
    {
        c = (char)fgetc(fp);
        if (c == EOF)
        {
            if (buf[counter - 1] != ',')
            {
                buf[counter] = ',';
                counter++;
            }
            buf[counter] = EOF;
            break;
        }
        buf[counter] = c;
        counter++;
    } while (1);

    char *rsbuf = (char *)calloc((size_t)counter, sizeof(char));
    memcpy(rsbuf, buf, (size_t)counter);
    bufSize = (uint64_t)counter;

    fclose(fp);
    return (char *)buf;
}

static unsigned char gethex(const char *s, char **endptr)
{
    while (isspace(*s))
        s++;
    return (unsigned char)strtoul(s, endptr, 16);
}

unsigned char *convert(const char *s, int *length)
{
    unsigned char *answer = malloc((strlen(s) + 1) / 3);
    unsigned char *p;
    for (p = answer; *s; p++)
        *p = gethex(s, (char **)&s);
    *length = (int)(p - answer);
    return answer;
}

char *processBuffer(char *buf)
{
    char *out = (char *)calloc(bufSize, sizeof(char));
    char *ccHex = calloc(16, sizeof(char));
    int hexCount = 0;
    int ccCount = 0;
    for (int i = 0; i < (int)bufSize; i++)
    {
        char *c = &buf[i];
        printf("%c", *c);
        int valid = 0;
        for (int j = 0; j < 21; j++)
        {
            if (*c == validChars[j])
                valid = 1;
        }
        if (valid)
        {
            ccHex[ccCount] = *c;
            ccCount++;
        }
        else if (*c == ',')
        {
            out[hexCount] = (char)*convert(ccHex, &ccCount);
            hexCount++;
            ccCount = 0;
        }
    }
    out[hexCount] = EOF;
    free(buf);
    return out;
}

void writeFile(const char *fileName, char *buf)
{
    FILE *fp = fopen(fileName, "wb");

    char done = 0;
    int count = 0;
    while (!done)
    {
        if (buf[count] == EOF)
        {
            done = 1;
        }
        else
        {
            if (buf[count] != 0x0)
                putc(buf[count], fp);
        }
        count++;
    }
}

int main(int argc, char *argv[])
{
    char *outName;

    if (argc < 2)
        exit(1);

    char *file = argv[1];

    if (argc >= 3)
    {
        bufSize = (uint64_t)atoi(argv[2]);
    }

    outName = calloc(strlen(file) + 4, sizeof(char));
    strcat(outName, file);
    strcat(outName, ".hex");

    if (!exists(file))
    {
        printf("file doesnt exist  %s\n", file);
        exit(1);
    }

    char *buf = loadFile(file);
    buf = processBuffer(buf);
    writeFile(outName, buf);
    free(buf);

    return 0;
}
