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
    FILE* p = fopen("songname.txt", "r");
    if (p == NULL)
    {
        printf("\033[1;31m⚠️  Couldn't open song library file.\033[0m\n");
        return;
    }

    printf("\033[38;5;219m╔══════════════════════════════════════════════════════╗\033[0m\n");
    printf("              \033[1;38;5;219m🎧  SONG LIBRARY\033[0m\n");
    printf("\033[38;5;219m╚══════════════════════════════════════════════════════╝\033[0m\n\n");

    char read[100];
    int count = 1;
    while (fgets(read, sizeof(read), p) != NULL)
    {
        read[strcspn(read, "\n")] = '\0';
        printf("\033[1;36m#%02d\033[0m\n", count++);

        printf("   \033[1;95m♪  Song:\033[0m      \033[38;5;220m%s\033[0m\n", read);
        fgets(read, sizeof(read), p);
        read[strcspn(read, "\n")] = '\0';
        printf("   \033[1;95m👤  Artist:\033[0m    \033[38;5;118m%s\033[0m\n", read);
        fgets(read, sizeof(read), p);
        read[strcspn(read, "\n")] = '\0';
        printf("   \033[1;95m⏱  Duration:\033[0m  \033[38;5;39m%s seconds\033[0m\n", read);

        printf("\033[2m-------------------------------------------------------\033[0m\n");
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
    fclose(p);
    return 0;
}