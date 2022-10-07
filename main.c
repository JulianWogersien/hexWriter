#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#ifdef WINDOWS
#include <direct.h>
#define cd _getcwd
#else
#include <unistd.h>
#define cd getcwd
#endif

uint64_t bufSize = 32768;

int exists(const char*);
char* loadFile(const char*);
char* processBuffer(char* buf);
unsigned char *convert(const char* s, int* length);
static unsigned char gethex(const char* s, char** endptr);
void writeFile(const char* fileName, char* buf);

int exists(const char* file)
{
    if(access(file, F_OK) == 0)
    {
        return 1;
    }
    return 0;
}

char* loadFile(const char* file)
{
    char* buf = (char*) malloc(bufSize);
    FILE* fp = fopen(file, "r");

    int counter = 0;
    char ch = ' ';
    while((ch = (char)fgetc(fp)) != EOF)
    {
        buf[counter] = ch;
        counter++;
    }
    buf[counter] = EOF;
    fclose(fp);
    return buf;
}

static unsigned char gethex(const char* s, char** endptr) {
    while (isspace(*s)) s++;
    return (unsigned char)strtoul(s, endptr, 16);
}

unsigned char* convert(const char* s, int* length) {
    unsigned char* answer = malloc((strlen(s) + 1) / 3);
    unsigned char* p;
    for (p = answer; *s; p++)
        *p = gethex(s, (char**)&s);
    *length = (int)(p - answer);
    return answer;
}

char* processBuffer(char* buf)
{
    char* out = (char*)calloc(bufSize, sizeof(char));
    char* cht = calloc(16, sizeof(char));
    char ch;
    char done = 0;
    int count = 0;
    int chCount = 0;
    while(!done)
    {
        ch = buf[count];
        if(ch == ',' || ch == '.')
        {
            out[count] = (char)*convert(cht, &chCount);
            chCount = 0;
        }else if(ch == EOF)
        {
            break;
        }else if(ch == ' ')
        {
        }else
        {
            cht[chCount] = ch;
            chCount++;
        }
        count++;
    }
    count++;
    out[count] = EOF;
    free(buf);
    return out;
}

void writeFile(const char* fileName, char* buf)
{
    FILE* fp = fopen(fileName, "w");

    char done = 0;
    int count = 0;
    while(!done)
    {
        if(buf[count] != 0x0)
            putc(buf[count], fp);
        if(buf[count] == EOF)
        {
            done = 1;
        }
        count++;
    }
}

int main(int argc, char *argv[])
{
    char* outName;

    if(argc < 2) exit(1);

    char* file = argv[1];

    if(argc >= 3){
        bufSize = (uint64_t)atoi(argv[2]);
    }

    outName = calloc(strlen(file) + 4, sizeof(char));
    strcat(outName, file);
    strcat(outName, ".gz");

    if(!exists(file)){
        printf("file doesnt exist  %s\n", file);
        exit(1);
    }

    char* buf = loadFile(file);
    buf = processBuffer(buf);
    writeFile(outName, buf);
    free(buf);

    return 0;
}
