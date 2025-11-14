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
        printf("\n");
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
                printf("\033[1;31m⚠️  Song doesn't exist in the library\033[0m\n");
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
            l[strlen(l) - 1] = '\0';
            if (equal(l, album))
            {
                f = 1;
            }
            strcat(l, "\n");
            strcat(data, l);
            fgets(l, 20, a);
            int i = atoi(l);
            if (f)
                sprintf(l, "%d", i + 1);
            else
                sprintf(l, "%d", i);
            strcat(l, "\n");
            strcat(data, l);
            for (int x = 1; x <= i; x++)
            {
                fgets(l, 20, a);
                strcat(data, l);
            }
            if (f)
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
        printf("\033[1;31m⚠️  Song '%s' doesn't exist in the library\033[0m\n", song);
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
        if (!f)
        {
            strcat(l, "\n");
            strcat(data, l);
        }
        fgets(l, 20, a);
        if (!f)
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
    free(data);
    if (!check)
    {
        printf("Album doesn't Exist to delete\n");
    }
}

void listAlbum(char *n, int all)
{
    if (exists(n) || all)
    {
        a = fopen("album.txt", "r");
        char l[30];

        printf("\033[2J\033[H"); // Clear screen
        printf("\n\033[1;34m╔══════════════════════════════════════════════╗\n");
        if (all)
            printf("║         🎵  ALL ALBUMS IN LIBRARY  🎵         ║\n");
        else
            printf("║           🎶  ALBUM DETAILS  🎶              ║\n");
        printf("╚══════════════════════════════════════════════╝\033[0m\n\n");

        while (fgets(l, 30, a) != NULL)
        {
            int f = 0;
            l[strlen(l) - 1] = '\0';
            if (equal(l, n) || all)
            {
                f = 1;
                printf("\n\033[1;33m📀 Album:\033[0m \033[1;97m%s\033[0m\n", l);
                printf("\033[90m──────────────────────────────────────────────\033[0m\n");
            }

            fgets(l, 30, a);
            int i = atoi(l);

            for (int x = 0; x < i; x++)
            {
                fgets(l, 30, a);
                if (f)
                {
                    l[strcspn(l, "\n")] = 0; // remove newline safely
                    printf("   \033[1;36m♪\033[0m  \033[1;37m%-25s\033[0m\n", l);
                }
            }

            if (f)
            {
                printf("\033[90m──────────────────────────────────────────────\033[0m\n");
                printf("   \033[1;33mTotal Songs:\033[0m \033[1;97m%d\033[0m\n\n", i);
            }

            if (f && !all)
                break;
        }

        fclose(a);

        printf("\033[1;34m╔══════════════════════════════════════════════╗\n");
        printf("║               ✨ END OF LIST ✨               ║\n");
        printf("╚══════════════════════════════════════════════╝\033[0m\n");
    }
    else
    {
        printf("\n\033[1;31m⚠ Album does not exist!\033[0m\n");
    }
}

void deleteSong(char *album, char *song)
{
    if (songExists(song))
    {
        int check = 0;
        a = fopen("album.txt", "r");
        if (!a)
        {
            printf("Cannot open album file\n");
            return;
        }
        char *data = (char *)malloc(10000);
        data[0] = '\0';
        char l[20];
        while (fgets(l, 20, a) != NULL)
        {
            int f = 0;
            l[strlen(l) - 1] = '\0';
            if (equal(l, album))
            {
                f = 1;
            }
            strcat(l, "\n");
            strcat(data, l);
            fgets(l, 20, a);
            int i = atoi(l);
            char *temp = (char *)malloc(20 * i * sizeof(char));
            temp[0] = '\0';
            for (int x = 1; x <= i; x++)
            {
                fgets(l, 20, a);
                l[strlen(l) - 1] = '\0';
                if (f && equal(l, song))
                {
                    check = 1;
                    continue;
                }
                strcat(temp, l);
                strcat(temp, "\n");
            }
            if (f && check && i > 0)
            {
                i--;
            }
            sprintf(l, "%d\n", i);
            strcat(l, temp);
            free(temp);
            strcat(data, l);
        }
        fclose(a);
        a = fopen("album.txt", "w");
        fprintf(a, "%s", data);
        fclose(a);
        free(data);
        if (!check)
        {
            printf("\033[1;31m⚠️  Song '%s' doesn't exist in the library\033[0m\n", song);
        }
    }
    else
    {
        printf("\033[1;31m⚠️  Song '%s' doesn't exist in the library\033[0m\n", song);
    }
}

void manage()
{
    char al[20];
    scanf(" %[^\n]", al);
    if (exists(al))
    {
        char c[20];
        scanf("\n%s", c);
        if (equal(c, "add"))
        {
            char s[20];
            scanf(" %[^\n]", s);
            addToAlbum(al, s);
            fprintf(songlog, "Manage %s %s %s\n", al, c, s);
        }
        else if (equal(c, "delete"))
        {
            char s[20];
            scanf(" %[^\n]", s);
            deleteSong(al, s);
            fprintf(songlog, "Manage %s %s %s\n", al, c, s);
        }
        else
        {
            printf("\033[1;31m❌ Unknown command:\033[0m \033[1;33m%s\033[0m\n", c);
            printf("\033[90mType 'help' to see available commands.\033[0m\n");

            fprintf(songlog, "Manage %s %s\n", al, c);
        }
    }
    else
    {
        printf("Album does not exist\n");
        fprintf(songlog, "Manage %s\n", al);
    }
}

char **addAlbum(char *n)
{
    if (exists(n))
    {
        int f = 0;
        a = fopen("album.txt", "r");
        char l[20];
        while (fgets(l, 20, a) != NULL)
        {
            f = 0;
            l[strlen(l) - 1] = '\0';
            if (equal(l, n))
            {
                f = 1;
            }
            fgets(l, 20, a);
            l[strlen(l) - 1] = '\0';
            int i = atoi(l);
            char **ar = (char **)malloc((i + 1) * sizeof(char *));
            if (f)
                ar[0] = strdup(l);
            for (int x = 1; x <= i; x++)
            {
                fgets(l, 20, a);
                l[strlen(l) - 1] = '\0';
                if (f)
                    ar[x] = strdup(l);
            }
            if (f)
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
        printf("\033[1;31m⚠️  Album '%s' doesn't exist in the library\033[0m\n", n);
        return NULL;
    }
}
