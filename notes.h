// notes.h
#ifndef NOTES_H
#define NOTES_H

#include "globals.h"

void addNote(char* title, char* content, int userId);
void deleteNote(int noteId);

#endif
