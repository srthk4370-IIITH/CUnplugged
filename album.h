#ifndef ALBUM_H
#define ALBUM_H

int exists(char *n);

void create(char *al);

void addToAlbum(char *album, char *song);

void listAlbum(char* n, int all);

void deleteAlbum(char *al);

void deleteSong(char *album, char *song);

void manage();

char** addAlbum(char* n);

#endif