#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "songControl.h"
#include "album.h"
FILE *a;

int exists(char *n)
{
    a = fopen("album.txt", "r");
    if (a != NULL)
    {
        char l[20];
        while (fgets(l, 20, a) != NULL)
        {
            l[strlen(l) - 1] = '\0';
            if (equal(l, n))
            {
                return 1;
            }
            fgets(l, 20, a);
            int i = atoi(l);
            for (int x = 1; x <= i; x++)
            {
                fgets(l, 20, a);
            }
        }
        fclose(a);
    }
    return 0;
}

void create(char *al)
{
    if (!exists(al))
    {
        a = fopen("album.txt", "a");
        fprintf(a, "%s\n", al);
        printf("How many Songs do you want to add?\n");
        int i;
        scanf("%d", &i);
        fprintf(a, "%d\n", i);
        for (int x = 1; x <= i; x++)
        {
            printf("Enter name of song %d\n", x);
            char n[20];
            scanf("\n%[^\n]", n);
            if (songExists(n))
            {
                fprintf(a, "%s\n", n);
            }
            else
            {
                printf("Song Doesn't exist in the Library\n");
                x--;
            }
        }
        fclose(a);
    }
    else
    {
        printf("Album already exists\n");
    }
}

void addToAlbum(char *album, char *song)
{
    if (songExists(song))
    {
        a = fopen("album.txt", "r");
        char *data = (char *)malloc(10000);
        data[0] = '\0';
        char l[20];
        while (fgets(l, 20, a) != NULL)
        {
            int f = 0;
            l[strlen(l)-1]= '\0';
            if (equal(l, album))
            {
                f = 1;
            }
            strcat(l, "\n");
            strcat(data, l);
            fgets(l, 20, a);
            int i = atoi(l);
            if(f)
                sprintf(l , "%d", i+1);
            else
                sprintf(l, "%d", i);
            strcat(l, "\n");
            strcat(data, l);
            for (int x = 1; x <= i; x++)
            {
                fgets(l, 20, a);
                strcat(data, l);
            }
            if(f)
            {
                strcat(song, "\n");
                strcat(data, song);
            }
        }
        fclose(a);
        a = fopen("album.txt", "w");
        fprintf(a, "%s", data);
        fclose(a);
    }
    else
    {
        printf("Song doesn't exist in the library");
    }
}

void deleteAlbum(char *al)
{
    int check = 0;
    a = fopen("album.txt", "r");
    char *data = (char *)malloc(10000);
    char l[20];
    while (fgets(l, 20, a) != NULL)
    {
        int f = 0;
        l[strlen(l) - 1] = '\0';
        if (equal(l, al))
        {
            f = 1;
        }
        if(!f)
        {
            strcat(l, "\n");
            strcat(data, l);
        }
        fgets(l, 20, a);
        if(!f)
            strcat(data, l);
        int i = atoi(l);
        for (int x = 1; x <= i; x++)
        {
            fgets(l, 20, a);
            if (f)
            {
                check = 1;
                continue;
            }
            strcat(data, l);
        }
    }
    fclose(a);
    a = fopen("album.txt", "w");
    fprintf(a, "%s", data);
    fclose(a);
    if (!check)
    {
        printf("Album doesn't Exist to delete\n");
    }
}

void listAlbum(char *n, int all)
{
    if(exists(n) || all)
    {
        a = fopen("album.txt", "r");
        char l[30];
        while(fgets(l, 30, a) != NULL)
        {
            int f;
            l[strlen(l)-1] = '\0';
            if(equal(l, n) || all)
            {
                f = 1;
                printf("Album name: %s\n", l);
            }
            fgets(l, 30, a);
            int i = atoi(l);
            for(int x=0; x<i; x++)
            {
                fgets(l, 30, a);
                if(f)
                {
                    printf("Song Name: %s", l);
                }
            }
            if(f && !all)
            {
                break;
            }
        }
        fclose(a);
    }
    else
    {
        printf("Album Does not exist\n");
    }
}

void deleteSong(char *album, char *song)
{
    if (songExists(song))
    {
        int check = 0;
        a = fopen("album.txt", "r");
        char *data = (char *)malloc(10000);
        char l[20];
        while (fgets(l, 20, a) != NULL)
        {
            int f = 0;
            l[strlen(l) -1]='\0';
            if (equal(l, album))
            {
                f = 1;
            }
            strcat(l, "\n");
            strcat(data, l);
            fgets(l, 20, a);
            int i = atoi(l);
            if(f && i!=0)
                sprintf(l, "%d", i-1);
            else
                sprintf(l, "%d", i);
            strcat(l, "\n");
            strcat(data, l);
            for (int x = 1; x <= i; x++)
            {
                fgets(l, 20, a);
                l[strlen(l) -1] = '\0';
                if (f && equal(l, song))
                {
                    check = 1;
                    continue;
                }
                strcat(l, "\n");
                strcat(data, l);
            }
        }
        fclose(a);
        a = fopen("album.txt", "w");
        fprintf(a, "%s", data);
        fclose(a);
        if (!check)
        {
            printf("Song doesn't exist in the album\n");
        }
    }
    else
    {
        printf("Song doesn't exist in the library\n");
    }
}

void manage()
{
    char al[20];
    scanf("%[^\n]", al);
    if (exists(al))
    {
        char c[20];
        scanf("\n%s", c);
        if (equal(c, "add"))
        {
            char s[20];
            scanf("%[^\n]", s);
            addToAlbum(al, s);
            fprintf(songlog, "Manage %s %s %s\n", al, c, s);
        }
        else if (equal(c, "delete"))
        {
            char s[20];
            scanf("%[^\n]", s);
            deleteSong(al, s);
            fprintf(songlog, "Manage %s %s %s\n", al, c, s);
        }
        else
        {
            printf("Command not found");
            fprintf(songlog, "Manage %s %s\n", al, c);
        }
    }
    else
    {
        printf("Album does not exist");
        fprintf(songlog, "Manage %s\n", al);
    }
}

char** addAlbum(char* n)
{
    if(exists(n))
    {
        int f=0;
        a = fopen("album.txt", "r");
        char l[20];
        while(fgets(l, 20, a) != NULL)
        {
            f =0;
            l[strlen(l) - 1]= '\0';
            if(equal(l, n))
            {
                f=1;
            }
            fgets(l, 20, a);
            l[strlen(l) - 1]= '\0';
            int i = atoi(l);
            char** ar = (char**)malloc((i+1) * sizeof(char*));
            if(f)
                ar[0] = strdup(l);
            for(int x=1; x<=i; x++)
            {
                fgets(l, 20, a);
                l[strlen(l) - 1]= '\0';
                if(f)
                    ar[x] = strdup(l);
            }
            if(f)
            {
                fclose(a);
                return ar;
            }
            free(ar);
        }
        return NULL;
    }
    else
    {
        printf("Album Doesn't exist\n");
        return NULL;
    }
}
