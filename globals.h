#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sqlite3.h>
#include <openssl/sha.h>

#define MAX_STRING 256
#define MAX_REMINDERS 1000
#define MAX_HASH_SIZE 1024
#define NOTIFICATION_INTERVAL 30

// Reminder structure for max heap
typedef struct Reminder {
    int id;
    int user_id;
    char text[MAX_STRING];
    time_t reminder_time;
    int priority;
    int notified;
} Reminder;

// Max Heap for priority-based reminders
typedef struct MaxHeap {
    Reminder* reminders[MAX_REMINDERS];
    int size;
    pthread_mutex_t lock;
} MaxHeap;

// Hash table entry for O(1) time-based retrieval
typedef struct HashEntry {
    int user_id;
    char key[MAX_STRING];
    void* data;
    struct HashEntry* next;
} HashEntry;

typedef struct HashTable {
    HashEntry* buckets[MAX_HASH_SIZE];
    pthread_mutex_t lock;
} HashTable;

// Linked list for sync queue (offline-first)
typedef struct SyncNode {
    char operation[32];
    char table[64];
    char data[MAX_STRING];
    time_t timestamp;
    struct SyncNode* next;
} SyncNode;

typedef struct SyncQueue {
    SyncNode* head;
    SyncNode* tail;
    int size;
    pthread_mutex_t lock;
} SyncQueue;

// User structure
typedef struct User {
    int id;
    char username[MAX_STRING];
    char password_hash[65];
    char user_code[16];
    time_t created_at;
} User;

// Journal entry
typedef struct Journal {
    int id;
    int user_id;
    char entry[1024];
    char date[32];
    time_t created_at;
} Journal;

// Mood entry
typedef struct Mood {
    int id;
    int user_id;
    char mood[MAX_STRING];
    char date[32];
    time_t created_at;
} Mood;

// Emergency contact
typedef struct Contact {
    int id;
    int user_id;
    char name[MAX_STRING];
    char relationship[MAX_STRING];
    char phone[32];
} Contact;

// Global database connection
extern sqlite3* db;
extern MaxHeap* reminder_heap;
extern HashTable* user_data_table;
extern SyncQueue* sync_queue;
extern int is_online;

// Function declarations

// Max Heap operations - O(log n)
MaxHeap* create_max_heap();
void heap_insert(MaxHeap* heap, Reminder* reminder);
Reminder* heap_extract_max(MaxHeap* heap);
void heap_heapify_up(MaxHeap* heap, int index);
void heap_heapify_down(MaxHeap* heap, int index);
void heap_destroy(MaxHeap* heap);

// Hash Table operations - O(1)
HashTable* create_hash_table();
unsigned int hash_function(const char* key);
void hash_insert(HashTable* table, const char* key, void* data, int user_id);
void* hash_get(HashTable* table, const char* key, int user_id);
void hash_delete(HashTable* table, const char* key, int user_id);
void hash_destroy(HashTable* table);

// Sync Queue operations (Linked List)
SyncQueue* create_sync_queue();
void sync_enqueue(SyncQueue* queue, const char* operation, const char* table, const char* data);
SyncNode* sync_dequeue(SyncQueue* queue);
void sync_process_queue(SyncQueue* queue);
void sync_destroy(SyncQueue* queue);

// Database operations
int init_database();
void close_database();
int create_tables();

// User authentication - SHA-256
void sha256_hash(const char* password, char* output_hash);
int register_user(const char* username, const char* password, char* user_code);
int login_user(const char* username, const char* password, User* user);

// Reminder operations
int add_reminder(int user_id, const char* text, time_t reminder_time, int priority);
int get_user_reminders(int user_id, Reminder** reminders, int* count);
int delete_reminder(int reminder_id);
int update_reminder_notified(int reminder_id);

// Journal operations
int add_journal(int user_id, const char* entry, const char* date);
int get_user_journals(int user_id, Journal** journals, int* count);
int get_journals_by_date(int user_id, const char* date, Journal** journals, int* count);

// Mood operations
int add_mood(int user_id, const char* mood, const char* date);
int get_user_moods(int user_id, Mood** moods, int* count);
int get_moods_by_date(int user_id, const char* date, Mood** moods, int* count);

// Contact operations
int add_contact(int user_id, const char* name, const char* relationship, const char* phone);
int get_user_contacts(int user_id, Contact** contacts, int* count);
int delete_contact(int contact_id);

// Multi-threaded notification scheduler
void* notification_thread(void* arg);
void start_notification_scheduler();
void check_and_notify_reminders();

// Utility functions
void generate_user_code(char* code);
time_t parse_time_string(const char* time_str);
void get_current_date(char* date_str);

#endif
