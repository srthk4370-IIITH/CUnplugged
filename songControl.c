#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "songControl.h"

FILE* songlog;

void addToFile(char* a, char* b, int d)
{
    FILE* p;
    p = fopen("songname.txt", "a");
    if(p)
    {
        fprintf(p, "%s\n", a);
        fprintf(p, "%s\n", b);
        fprintf(p, "%d\n", d);
    }
    fclose(p);
}

int equal(char* a, char* b)
{
    while(*a && *b)
    {
        if(tolower(*a) != tolower(*b))
        {
            return 0;
        }
        a++;
        b++;
    }
    return !(*a) && !(*b);
}

void listSongs()
{
    FILE* p;
    p = fopen("songname.txt", "r");
    if(p != NULL)
    {
        char read[30];
        while(fgets(read, 30, p) != NULL)
        {
            printf("Song name: %s\n", read);
            fgets(read, 30, p);
            printf("Artist: %s\n", read);
            fgets(read, 30, p);
            printf("Duration: %s", read);
        }
    }
    else{
        printf("MAA KI CHUT");
    }
    fclose(p);
}

int songExists(char* n)
{
    FILE* p;
    p = fopen("songname.txt", "r");
    if(p != NULL)
    {
        char read[30];
        while(fgets(read, 30, p) != NULL)
        {
            read[strlen(read)-1] = '\0';
            if(equal(read, n))
            {
                return 1;
            }
            fgets(read, 20, p);
            fgets(read, 20, p);
        }
    }
    return 0;
}