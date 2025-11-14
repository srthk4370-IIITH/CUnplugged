C-Unplugged
This project is a terminal-based music player built as part of my C Programming – Assignment 2.
The goal was to design a multi-file C program that handles file operations, linked lists, threading, user input, and formatted terminal output.
I ended up turning it into a small but fully usable text-mode music player.

It works on two terminals:
one for entering commands, and a second one that displays the “player screen” with the current song, timers, and progress visuals.
This keeps the interface clean and avoids mixing input with output.
The player loads songs from a simple text file.
Albums follow a similar structure, allowing multiple songs to be added to the queue at once.

The program uses a doubly linked list to manage the queue and pthreads to handle real-time song playback without blocking user input.
ANSI escape codes are used for UI formatting.

Available commands include:

add <song> — Add a song to the queue

list — Show songs currently in the queue

next — Skip to the next song

prev — Go back to the previous song

pause — Pause playback

resume — Resume

playat <n> — Jump to the nth song

clear — Empty the queue

listall — Display the entire song library

listalbum <name> — List songs in an album

playalbum <name> — Load an album to the queue

repeat — Toggle repeat mode

loop — Toggle looping through the queue

exit — Close the program

Typing help prints a short guide with all commands.