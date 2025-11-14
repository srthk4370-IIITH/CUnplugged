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
    char *name;
    char *singer;
    int dur;
} Song;

typedef struct List
{
    Song *s;
    struct List *p;
    struct List *n;
} List;

List *head = NULL;
List *tail = NULL;
List *curr = NULL;
FILE *ter1;

void terminal()
{
    FILE *pathfile;
    const char *terminals[] = {
        "konsole --profile Small  -e bash -c 'tty > %s; exec bash' &",
        "gnome-terminal --geometry=80x24+100+100 -- bash -c 'tty > %s; exec bash' &",
        "xdg-terminal -- bash -c 'tty > /tmp/second_tty_path; exec bash' &",
        "x-terminal-emulator -e bash -c 'tty > %s; exec bash' &",
        "xterm -hold -e bash -c 'tty > %s; exec bash' &",
        NULL};
    int i = 0;
    const char *f = "/tmp/second_tty_path";
    do
    {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), terminals[i++], f);
        system(cmd);
        sleep(1);
        printf("\033[2J\033[H Waiting for the terminal to open....Please Wait");
        pathfile = fopen(f, "r");

        char path[128];
        if (!fgets(path, sizeof(path), pathfile))
        {
            fprintf(stderr, "Failed to read tty path\n");
            fclose(pathfile);
        }
        fclose(pathfile);
        path[strcspn(path, "\n")] = '\0';

        ter1 = fopen(path, "w");
    } while (ter1 == NULL && i < 4);
    if (!ter1)
    {
        printf("Failed To open Secondary Terminal so printing will be here only");
        ter1 = stdout;
    }
}

/*void printSong(Song* song)
{
    int i = 0;
    while(song != NULL && i <= song -> dur && !skip && !ex)
    {
        if(playing && song != NULL)
        {
            fprintf(ter1, "\033[2J\033[H");
            fprintf(ter1, "            \033[1;21;32mSONG PLAYER ◀ ⏸ ▶:\n\033[0m");
            fprintf(ter1, "\033[1;36;4mCurrently Playing:\033[0m \033[33m%s\033[0m\n", song -> name);
            fprintf(ter1, "\033[1;36mBy:\033[0m \033[33m%s\033[0m\n", song -> singer);
            fprintf(ter1, "\033[1;36m%d:%d\033[0m", i/60, i%60);
            for(int x=0; x<=((float)i)/(song->dur)*100; x++)
            {
                fprintf(ter1, "=");
            }
            for(int x=((float)i)/(song->dur)*100 + 1; x<=100; x++)
            {
                fprintf(ter1, "-");
            }
            fprintf(ter1, "\033[1;36m%d:%d\n\033[0m", (song->dur)/60, (song->dur)%60);
            i++;
            sleep(1);
        }
    }
}*/

void printSong(Song *song)
{
    int i = 0;
    while (song != NULL && i <= song->dur && !skip && !ex)
    {
        if (playing && song != NULL)
        {
            fprintf(ter1, "\033[2J\033[H");
            fprintf(ter1, "\n\033[1;32m╔══════════════════════════════════════════╗\n");
            fprintf(ter1, "║            \033[1;97mSONG PLAYER ◀ ⏸ ▶\033[0;32m              ║\n");
            fprintf(ter1, "╚══════════════════════════════════════════╝\033[0m\n\n");
            fprintf(ter1, "  \033[1;36;4mCurrently Playing:\033[0m \033[1;33m%s\033[0m\n", song->name);
            fprintf(ter1, "  \033[1;36mArtist:\033[0m \033[1;33m%s\033[0m\n\n", song->singer);

            int len = 40;
            int progress = ((float)i / song->dur) * len;

            fprintf(ter1, "  \033[1;36m[%02d:%02d]\033[0m ", i / 60, i % 60);

            for (int j = 0; j < len; j++)
            {
                if (j < progress)
                {
                    int color = 32 + (j % 6);
                    fprintf(ter1, "\033[1;%dm●\033[0m", color);
                }
                else
                {
                    fprintf(ter1, "\033[90m·\033[0m");
                }
            }

            fprintf(ter1, " \033[1;36m[%02d:%02d]\033[0m\n", song->dur / 60, song->dur % 60);

            fprintf(ter1, "\n  \033[1;90m──────────────────────────────────────────────\033[0m\n");

            fflush(ter1);
            i++;
            sleep(1);
        }
    }
}

void *play(void *arg)
{
    while (!ex)
    {
        if (curr == NULL)
        {
            pthread_mutex_lock(&lock);
            curr = head;
            playing = 1;
            pthread_mutex_unlock(&lock);
            if (ex)
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
            Song *temp;
            pthread_mutex_lock(&lock);
            if (curr != NULL)
            {
                temp = curr->s;
            }
            pthread_mutex_unlock(&lock);
            printSong(temp);
            if (!skip && curr != NULL && !rep && !loop)
            {
                curr = curr->n;
            }
            else if (skip)
            {
                skip = 0;
            }
            if (rep)
            {
                rep = 0;
            }
        }
        playing = 1;
        if (ex)
        {
            system("pkill konsole");
            system("pkill gnome-terminal");
            fclose(ter1);
            pthread_exit(NULL);
        }
    }
    system("pkill konsole");
    exit(0);
}

void addToList(Song *song)
{
    if (head == NULL)
    {
        head = (List *)malloc(sizeof(List));
        head->s = song;
        head->n = NULL;
        head->p = head;
        tail = head;
    }
    else
    {
        List *newNode = (List *)malloc(sizeof(List));
        newNode->s = song;
        newNode->n = NULL;
        newNode->p = NULL;
        tail->n = newNode;
        newNode->p = tail;
        head->p = newNode;
        tail = newNode;
    }
}

void add(char ch[])
{
    fprintf(songlog, "Play %s\n", ch);
    Song *news = (Song *)malloc(sizeof(Song));
    FILE *ptr;
    int f = 0;
    ptr = fopen("songname.txt", "r");
    if (!(ptr == NULL))
    {
        char l[20];
        while (fgets(l, 20, ptr) != NULL)
        {
            l[strlen(l) - 1] = '\0';
            if (equal(l, ch))
            {
                news->name = strdup(l);
                fgets(l, 20, ptr);
                l[strlen(l) - 1] = '\0';
                news->singer = strdup(l);
                fgets(l, 20, ptr);
                l[strlen(l) - 1] = '\0';
                news->dur = atoi(l);
                addToList(news);
                f = 1;
                printf("\n\033[1;93m⚡  TRACK QUEUED:\033[0m  \033[1;38;5;51m%s\033[0m  \033[38;5;118m🔥\033[0m\n", ch);
                break;
            }
            else
            {
                fgets(l, 20, ptr);
                fgets(l, 20, ptr);
            }
        }
        fclose(ptr);
        if (!f)
        {
            printf("\033[1;31m⚠️  Song '%s' doesn't exist in the library\033[0m\n", ch);
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

    printf("\n\033[38;5;39m╔═══════════════════════════════════════════════╗\033[0m\n");
    printf("  \033[1;38;5;220m🎵  CURRENT PLAYLIST\033[0m\n");
    printf("\033[38;5;39m╚═══════════════════════════════════════════════╝\033[0m\n\n");

    List *temp = head;

    if (temp == NULL)
    {
        printf("\033[2;3m(No songs added yet — your playlist is empty.)\033[0m\n\n");
        return;
    }

    int i = 1;
    do
    {
        if (temp == curr)
        {
            printf("  \033[1;38;5;46m▶  %2d. \033[38;5;51m%s\033[0m \033[2m(currently playing)\033[0m\n", i++, temp->s->name);
        }
        else
        {
            printf("    \033[1;38;5;45m%2d.\033[0m \033[38;5;250m%s\033[0m\n", i++, temp->s->name);
        }
        temp = temp->n;
    } while (temp != NULL);

    printf("\n\033[38;5;39m───────────────────────────────────────────────\033[0m\n");
    printf("\033[2;3mTip:\033[0m Use \033[1mplay\033[0m, \033[1mpause\033[0m, or \033[1mnext\033[0m commands to control playback.\n\n");
}

void next()
{
    fprintf(songlog, "Next\n");

    if (curr != NULL && curr->n != NULL)
    {
        printf("\n\033[1;36m⟶ Moving to Next Song...\033[0m\n");
        curr = curr->n;
    }
    else
    {
        printf("\n\033[1;31m⨯ No next song available.\033[0m "
               "\033[90m(End of queue)\033[0m\n");
    }

    skip = 1;
}

void prev()
{
    fprintf(songlog, "Previous\n");

    if (curr != NULL && curr->p != NULL && curr!=head)
    {
        printf("\n\033[1;36m⟵ Moving to Previous Song...\033[0m\n");
        curr = curr->p;
    }
    else
    {
        printf("\n\033[1;31m⨯ No previous song available.\033[0m "
               "\033[90m(Start of queue)\033[0m\n");
    }

    skip = 1;
}


void clear()
{
    playing = 0;
    skip = 1;
    if (head != NULL)
    {
        List *temp = head;
        while (temp != NULL)
        {
            List *next = temp->n;
            free(temp->s->name);
            free(temp->s->singer);
            free(temp->s);
            free(temp);
            temp = next;
        }
        head = NULL;
        curr = NULL;
    }
    fprintf(ter1, "\033[2J\033[H");
    fprintf(ter1, "\n\033[1;38;5;45m♪ ♪ ♪  No song currently playing  ♪ ♪ ♪\033[0m\n");
    fprintf(ter1, "\033[3;38;5;111m(open main terminal to play something...)\033[0m\n");
}

void playAt(int i)
{
    List *temp = head;
    for (int x = 1; x < i && temp != NULL; x++)
    {
        temp = temp->n;
    }
    if (temp != NULL)
    {
        curr = temp;
        skip = 1;
    }
    else
    {
        printf("Song not added yet\n");
    }
}

void printHelp()
{
    printf("\033[38;5;39m╔══════════════════════ HELP MENU ══════════════════════╗\033[0m\n");
    printf("\033[1;33m│ \033[1;97mPlay <song_name>\033[0;33m       : Add song to queue and play it.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mList\033[0;33m                  : List current queue.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mNext\033[0;33m                  : Skip to next song.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mPrev / Previous\033[0;33m      : Go to previous song.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mPause\033[0;33m                 : Pause current song.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mResume\033[0;33m                : Resume paused song.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mRemove <index>\033[0;33m         : Remove a song from queue.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mPlayAt <index>\033[0;33m        : Play song at specific position.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mListAll\033[0;33m                : Show all songs in library.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mAddSong <Song name> *Enter* <Artist Name> *Enter* <Duration>\033[0;33m : Add new song to library.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mPlayAlbum <album_name>\033[0;33m : Play all songs from an album.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mListAlbum <album_name>\033[0;33m : List songs in an album.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mListAllAlbums\033[0;33m          : List all albums.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mCreateAlbum <name>\033[0;33m     : Create a new album.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mDeleteAlbum <name>\033[0;33m     : Delete an album.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mClear\033[0;33m                 : Clear current queue.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mRepeat\033[0;33m                : Toggle repeat for current song once.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mLoop\033[0;33m                  : Toggle loop for current song forever.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mManageAlbum <album_name> *Enter* add <song_name>\033[0;33m: Add song to the album.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mManageAlbum <album_name> *Enter* delete <song_name>\033[0;33m: Delete song from the album.\033[0m\n");
    printf("\033[1;33m│ \033[1;97mExit\033[0;33m                  : Exit the music player.\033[0m\n");
    printf("\033[38;5;39m╚══════════════════════════════════════════════════════╝\033[0m\n");
}

void *input(void *arg)
{
    char in[20];
    while (1)
    {
        printf("\033[1;92mAt your command> \033[0m");
        fflush(stdout);
        scanf(" %s", in);
        pthread_mutex_lock(&lock);
        if (equal(in, "Play") || equal(in, "Add"))
        {
            scanf(" %[^\n]", in);
            add(in);
        }
        else if (equal(in, "help"))
        {
            printHelp();
            fprintf(songlog, "Help\n");
        }
        else if (equal(in, "List"))
        {
            list();
            fprintf(songlog, "List Songs");
        }
        else if (equal(in, "next"))
        {
            next();
        }
        else if (equal(in, "prev") || equal(in, "Previous"))
        {
            prev();
        }
        else if (equal(in, "pause"))
        {
            playing = 0;
            fprintf(ter1, "\033[2J\033[H");
            fprintf(ter1, "\n\033[1;33m╔══════════════════════════════════════════╗\n");
            fprintf(ter1, "║              \033[1;97m⏸ SONG PAUSED\033[0;33m               ║\n");
            fprintf(ter1, "╚══════════════════════════════════════════╝\033[0m\n\n");

            fprintf(ter1, "  \033[1;36mType resume to start the music.\033[0m\n");
            fprintf(ter1, "  \033[90m──────────────────────────────────────────────\033[0m\n");
            fprintf(ter1, "\n  \033[1;5;33m● ● ● Paused ● ● ●\033[0m\n");

            fprintf(songlog, "Pause\n");
        }
        else if (equal(in, "Resume"))
        {
            playing = 1;
            fprintf(songlog, "Resume\n");
        }
        else if (equal(in, "Remove"))
        {
            int i;
            scanf("%d", &i);
            fprintf(songlog, "Remove %d\n", i);
            List *temp = head;
            for (int x = 1; x < i && temp != NULL; x++)
            {
                temp = temp->n;
            }
            if (temp != NULL)
            {
                if (temp == curr)
                {
                    next();
                }
                if (i != 1)
                {
                    temp->p->n = temp->n;
                }
                else
                {
                    head = head->n;
                }
                if (temp->n != NULL)
                    temp->n->p = temp->p;
                else
                    head->p = temp->p;
            }
            else
            {
                printf("\033[1;33m⚠️  Song not added yet.\033[0m\n");
            }
        }
        else if (equal(in, "PlayAt"))
        {
            int i;
            scanf("%d", &i);
            playAt(i);
            fprintf(songlog, "Play Song At %d", i);
        }
        else if (equal(in, "ListAll"))
        {
            fprintf(songlog, "list All Songs\n");
            listSongs();
        }
        else if (equal(in, "addSong"))
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
        else if (equal(in, "playAlbum"))
        {
            char n[20];
            scanf(" %[^\n]", n);
            fprintf(songlog, "Add Album %s\n", n);
            char **ar = addAlbum(n);
            if (ar != NULL)
            {
                int i = atoi(ar[0]);
                for (int x = 1; x <= i; x++)
                {
                    add(ar[x]);
                }
            }
            else
            {
                free(ar);
            }
        }
        else if (equal("manageAlbum", in))
        {
            manage();
        }
        else if (equal("createalbum", in))
        {
            char n[20];
            scanf(" %[^\n]", n);
            fprintf(songlog, "Create Album %s\n", n);
            create(n);
        }
        else if (equal("deleteAlbum", in))
        {
            char n[20];
            scanf(" %[^\n]", n);
            fprintf(songlog, "Delete Album %s\n", n);
            deleteAlbum(n);
        }
        else if (equal(in, "ListAlbum"))
        {
            char n[20];
            scanf(" %[^\n]", n);
            listAlbum(n, 0);
            fprintf(songlog, "List Album %s\n", n);
        }
        else if (equal(in, "ListAllAlbums"))
        {
            listAlbum("", 1);
            fprintf(songlog, "List All Album\n");
        }
        else if (equal(in, "Clear"))
        {
            clear();
            printf("Cleared Queue\n");
            fprintf(songlog, "Clear Queue\n");
        }
        else if (equal(in, "repeat"))
        {
            rep = !rep;
        }
        else if (equal(in, "Loop"))
        {
            loop = !loop;
        }
        else if (equal(in, "exit"))
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
            printf("\033[1;31m❌ Unknown command:\033[0m \033[1;33m%s\033[0m\n", in);
            printf("\033[90mType 'help' to see available commands.\033[0m\n");
        }
        pthread_mutex_unlock(&lock);
    }
}

int main()
{
    songlog = fopen("songlog.txt", "a");
    if (!songlog)
    {
        printf("Trouble opening songlog file");
        exit(1);
    }
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(songlog, NULL, _IONBF, 0);
    terminal();
    setvbuf(ter1, NULL, _IONBF, 0);
    fprintf(ter1, "\033[2J\033[H");
    fprintf(ter1, "\033[1;91m╔════════════════════════════════════════════╗\033[0m\n");
    fprintf(ter1, "\033[1;97m║  ⚠️ Do Not Input Here! Use Original Terminal ║\033[0m\n");
    fprintf(ter1, "\033[1;91m╚════════════════════════════════════════════╝\033[0m\n");
    printf("\033[2J\033[H");
    printf("\033[1;96m╔════════════════════════════════════════════╗\033[0m\n");
    printf("\033[1;97m║         Please Input Your Command:        ║\033[0m\n");
    printf("\033[1;96m╚════════════════════════════════════════════╝\033[0m\n");
    printf("\033[1;36mType \033[1;33mhelp\033[1;36m to see all available commands.\033[0m\n\n");

    pthread_mutex_init(&lock, NULL);
    pthread_create(&t1, NULL, play, NULL);
    pthread_create(&t2, NULL, input, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}