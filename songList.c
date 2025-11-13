#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "songControl.h"
#include "album.h"

int playing = 1;
int skip = 0;
int rep = 0, loop = 0;
volatile int ex = 0;
pthread_mutex_t lock;
pthread_t t1, t2;

typedef struct Song
{
    char* name;
    char* singer;
    int dur;
}Song;

typedef struct List
{
    Song* s;
    struct List* p;
    struct List* n;
}List;

List* head = NULL;
List* curr = NULL;
FILE* ter1;

void terminal()
{
    FILE* pathfile;
    const char* terminals[] = {
        "konsole --profile Small  -e bash -c 'tty > %s; exec bash' &",
        "gnome-terminal --geometry=80x24+100+100 -- bash -c 'tty > %s; exec bash' &",
        "xdg-terminal -- bash -c 'tty > /tmp/second_tty_path; exec bash' &",
        "x-terminal-emulator -e bash -c 'tty > %s; exec bash' &",
        "xterm -hold -e bash -c 'tty > %s; exec bash' &",
        NULL
    };
    int i =0;
    const char* f = "/tmp/second_tty_path";
    do
    {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), terminals[i++], f);
        system(cmd);
        sleep(1);
        printf("\033[2J\033[H Waiting for the terminal to open....Please Wait");
        pathfile = fopen(f, "r");

        char path[128];
        if (!fgets(path, sizeof(path), pathfile)) {
            fprintf(stderr, "Failed to read tty path\n");
            fclose(pathfile);
        }
        fclose(pathfile);
        path[strcspn(path, "\n")] = '\0';

        ter1 = fopen(path, "w");
    }while(ter1 == NULL && i<4);
    if(!ter1)
    {
        printf("Failed To open Secondary Terminal so printing will be here only");
        ter1 = stdout;
    }
}


void printSong(Song* song)
{
    int i = 0;
    while(i <= song -> dur && !skip && !ex)
    {
        if(playing)
        {
            fprintf(ter1, "\033[2J\033[H");
            fprintf(ter1, "Currently Playing: %s\n", song -> name);
            fprintf(ter1, "By: %s\n", song -> singer);
            fprintf(ter1, "%d:%d", i/60, i%60);
            for(int x=0; x<=((float)i)/(song->dur)*100; x++)
            {
                fprintf(ter1, "=");
            }
            for(int x=((float)i)/(song->dur)*100 + 1; x<=100; x++)
            {
                fprintf(ter1, "-");
            }
            fprintf(ter1, "%d:%d\n", (song->dur)/60, (song->dur)%60);
            i++;
            sleep(1);
        }
    }
}

void* play(void* arg)
{
    while(!ex)
    {
        if(curr == NULL)
        {
            pthread_mutex_lock(&lock);
            curr = head;
            playing = 1;
            pthread_mutex_unlock(&lock);
            if(ex)
            {
                system("pkill konsole");
                system("pkill gnome-terminal");
                fclose(ter1);
                pthread_exit(NULL);
            }
        }
        else
        {
            skip = 0;
            Song* temp;
            pthread_mutex_lock(&lock);
            if(curr  != NULL)
            {
                temp = curr -> s;
            }
            printSong(temp);
            if(!skip && curr != NULL &&!rep &&!loop)
            {
                curr = curr -> n;
            }
            else if(skip)
            {
                skip = 0;
            }
            if(rep)
            {
                rep = 0;
            pthread_mutex_unlock(&lock);
            }
        }
        playing =1;
        if(ex)
        {
            system("pkill konsole");
            system("pkill gnome-terminal");
            fclose(ter1);
            pthread_exit(NULL);
        }
    }
    printf("here");
    system("pkill konsole");
    exit(0);
}

void addToList(Song* song)
{
    if(head == NULL)
    {
        head = (List*)malloc(sizeof(List));
        head -> s = song;
        head -> n = NULL;
        head -> p = head;
    }
    else
    {
        List* temp = head;
        while(temp -> n != NULL)
        {
            temp = temp -> n;
        }
        List* newNode = (List*)malloc(sizeof(List));
        newNode -> s = song;
        newNode -> n = NULL;
        newNode -> p = NULL;
        temp -> n = newNode;
        newNode -> p = temp;
        head -> p = newNode;
    }
}

void add(char ch[])
{
    fprintf(songlog, "Play %s\n", ch);
    Song *news = (Song*)malloc(sizeof(Song));
    FILE *ptr;
    int f = 0;
    ptr = fopen("songname.txt", "r");
    if(!(ptr == NULL))
    {
        char l[20];
        while(fgets(l, 20, ptr) != NULL)
        {
            l[strlen(l)-1] ='\0';
            if(equal(l, ch))
            {
                news -> name = strdup(l);
                fgets(l, 20, ptr);
                l[strlen(l)-1] ='\0';
                news -> singer = strdup(l);
                fgets(l, 20, ptr);
                l[strlen(l)-1] ='\0';
                news -> dur = atoi(l);
                addToList(news);
                f=1;
                printf("%s added to the queue!\n", ch);
                break;
            }
            else
            {
                fgets(l, 20, ptr);
                fgets(l, 20, ptr);
            }
        }
        fclose(ptr);
        if(!f)
        {
            printf("Song doesn't exist in the library\n");
        }
    }
    else
    {
        printf("Error file did not open\n");
        exit(1);
    }
}

void list()
{
    fprintf(songlog, "List Song\n");
    printf("Listing the songs:\n");
    List* temp = head;
    if(temp == NULL)
    {
        printf("No Songs Added\n");
    }
    else
    {
        int i=1;
        do
        {
            if(temp == curr)
            {
                printf("%d. %s*\n", i++, temp -> s -> name);
            }
            else
            {
                printf("%d. %s\n", i++, temp -> s -> name);
            }
            temp = temp -> n;
        }while(temp != NULL);
    }
}

void next()
{
    fprintf(songlog, "Next\n");
    if(curr != NULL)
    {
        curr = curr -> n;
    }
    skip =1;
}

void prev()
{
    fprintf(songlog, "Previous\n");
    if(curr != NULL)
    {
        curr = curr -> p;
    }
    skip =1;
}

void clear()
{
    playing = 0;
    skip = 1;
    if(head!= NULL)
    {
        List* temp = head;
        while(temp != NULL)
        {
            List* next = temp -> n;
            free(temp);
            temp = next;
        }
        head = NULL;
        curr = NULL;
    }
    fprintf(ter1, "\033[2J\033[HNo Song To Play Currently! Give input on the original terminal");
}

void playAt(int i)
{
    List* temp = head;
    for(int x=1; x<i && temp!=NULL; x++)
    {
        temp = temp -> n;
    }
    if(temp != NULL)
    {
        curr = temp;
        skip = 1;
    }
    else
    {
        printf("Song not added yet\n");
    }
}

void* input(void* arg)
{
    char in[20];
    while(1)
    {
        scanf("%s", in);
        pthread_mutex_lock(&lock);
        if(equal(in, "Play"))
        {
            scanf(" %[^\n]", in);
            add(in);
        }
        else if(equal(in, "List"))
        {
            list();
            fprintf(songlog, "List Songs");
        }
        else if(equal(in, "next"))
        {
            next();
        }
        else if(equal(in, "prev") || equal(in, "Previous"))
        {
            prev();
        }
        else if(equal(in, "pause"))
        {
            playing = 0;
            fprintf(ter1, "\nSong Paused");
            fprintf(songlog, "Pause\n");
        }
        else if(equal(in, "Resume"))
        {
            playing = 1;
            fprintf(songlog, "Resume\n");
        }
        else if(equal(in, "Remove"))
        {
            int i;
            scanf("%d", &i);
            fprintf(songlog, "Remove %d\n", i);
            List* temp = head;
            for(int x=1; x<i && temp !=NULL; x++)
            {
                temp = temp -> n;
            }
            if(temp != NULL)
            {
                if(temp == curr)
                {
                    next();
                }
                if(i != 1)
                {
                    temp -> p -> n = temp -> n;
                }
                else
                {
                    head = head -> n;
                }
                if(temp -> n != NULL)
                    temp -> n -> p = temp -> p;
                else
                    head -> p = temp -> p;   
            }
            else{
                printf("Song not added yet");
            }
        }
        else if(equal(in, "PlayAt"))
        {
            int i;
            scanf("%d", &i);
            playAt(i);
            fprintf(songlog, "Play Song At %d", i);
        }
        else if(equal(in, "ListAll"))
        {
            fprintf(songlog, "list All Songs\n");
            listSongs();
        }
        else if(equal(in, "addSong"))
        {
            char a[20];
            char b[20];
            int d;
            scanf("\n%[^\n]", a);
            scanf("\n%[^\n]", b);
            scanf("%d", &d);
            addToFile(a, b, d);
            fprintf(songlog, "Add Song to File: %s %s %d", a, b, d);
        }
        else if(equal(in, "playAlbum"))
        {
            char n[20];
            scanf(" %[^\n]", n);
            fprintf(songlog, "Add Album %s\n", n);
            char** ar = addAlbum(n);
            if(ar != NULL)
            {
                int i = atoi(ar[0]);
                for(int x=1; x<=i; x++)
                {
                    add(ar[x]);
                }
            }
            else
            {
                printf("Album not formed");
                free(ar);
            }
        }
        else if(equal("manageAlbum", in))
        {
            manage();
        }
        else if(equal("createalbum", in))
        {
            char n[20];
            scanf(" %[^\n]", n);
            fprintf(songlog, "Create Album %s\n", n);
            create(n);
        }
        else if(equal("deleteAlbum", in))
        {
            char n[20];
            scanf(" %[^\n]", n);
            fprintf(songlog, "Delete Album %s\n", n);
            deleteAlbum(n);
        }
        else if(equal(in, "ListAlbum"))
        {
            char n[20];
            scanf(" %[^\n]", n);
            listAlbum(n, 0);
            fprintf(songlog, "List Album %s\n", n);
        }
        else if(equal(in, "ListAllAlbums"))
        {
            listAlbum("", 1);
            fprintf(songlog, "List All Album\n");
        }
        else if(equal(in, "Clear"))
        {
            clear();
            printf("Cleared Queue\n");
            fprintf(songlog, "Clear Queue\n");
        }
        else if(equal(in, "repeat"))
        {
            rep = !rep;
        }
        else if(equal(in, "Loop"))
        {
            loop = !loop;
        }
        else if(equal(in, "exit"))
        {
            fprintf(songlog, "Exit\n\n\n");
            ex = 1;
            pthread_mutex_unlock(&lock);
            pthread_mutex_destroy(&lock);
            fclose(songlog);
            pthread_exit(NULL);
        }
        else
        {
            printf("Command Not Found\n");
        }
        pthread_mutex_unlock(&lock);
    }
}

void main()
{
    songlog = fopen("songlog.txt", "a");
    if(!songlog)
    {
        printf("Trouble opening songlog file");
        exit(1);
    }
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(songlog, NULL, _IONBF, 0);
    terminal();
    printf("\033[2J\033[H Please Input here:");
    setvbuf(ter1, NULL, _IONBF, 0);
    fprintf(ter1, "\033[0;0H Do Not Input Here! Print In your Original Konsole!");
    pthread_mutex_init(&lock, NULL); 
    pthread_create(&t1, NULL, play, NULL);
    pthread_create(&t2, NULL, input, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}