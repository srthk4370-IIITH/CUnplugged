#ifndef SONGCONTROL_H
#define SONGCONTROL_H

extern FILE* songlog;

void addToFile(char* , char* b, int d);

void listSongs();

int equal(char* a, char* b);

int songExists(char* n);

#endif