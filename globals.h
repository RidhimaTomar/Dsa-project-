#ifndef GLOBALS_H
#define GLOBALS_H

#include <time.h>

#define PORT 8080
#define MAX_USERS 100
#define MAX_REMINDERS 100
#define MAX_NOTES 100
#define HASH_SIZE 50

// User structure
typedef struct User {
    char username[50];
    char password[50];
    int id;
} User;

// Priority Queue Node for Reminders
typedef struct ReminderNode {
    int id;
    char text[256];
    time_t timestamp;
    int priority;
    int userId;
} ReminderNode;

// Linked List Node for Notes/Journals
typedef struct NoteNode {
    int id;
    char title[100];
    char content[1000];
    time_t created;
    int userId;
    struct NoteNode* next;
} NoteNode;

// Hash Table for Time-indexed reminders
typedef struct HashNode {
    ReminderNode* reminders[MAX_REMINDERS];
    int count;
} HashNode;

// Global variables
extern User users[MAX_USERS];
extern int userCount;
extern ReminderNode reminderQueue[MAX_REMINDERS];
extern int reminderCount;
extern NoteNode* noteListHead;
extern HashNode hashTable[HASH_SIZE];
extern int noteIdCounter;
extern int reminderIdCounter;

#endif
