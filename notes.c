#include "notes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NoteNode* noteListHead = NULL;  // define global variable
int noteIdCounter = 1;          // define global variable

void addNote(char* title, char* content, int userId) {
    NoteNode* newNode = (NoteNode*)malloc(sizeof(NoteNode));
    newNode->id = noteIdCounter++;
    strcpy(newNode->title, title);
    strcpy(newNode->content, content);
    newNode->created = time(NULL);
    newNode->userId = userId;
    newNode->next = noteListHead;
    noteListHead = newNode;
}

void deleteNote(int noteId) {
    NoteNode* curr = noteListHead;
    NoteNode* prev = NULL;
    while (curr != NULL) {
        if (curr->id == noteId) {
            if (prev == NULL) {
                noteListHead = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
