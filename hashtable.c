#include "globals.h"

HashTable* create_hash_table() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    for (int i = 0; i < MAX_HASH_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    pthread_mutex_init(&table->lock, NULL);
    return table;
}

unsigned int hash_function(const char* key) {
    unsigned int hash = 5381;
    int c;
    int i = 0;
    while ((c = key[i++]) != '\0') {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % MAX_HASH_SIZE;
}

void hash_insert(HashTable* table, const char* key, void* data, int user_id) {
    pthread_mutex_lock(&table->lock);
    
    unsigned int index = hash_function(key);
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    
    entry->user_id = user_id;
    my_strncpy(entry->key, key, MAX_STRING - 1);
    entry->key[MAX_STRING - 1] = '\0';
    entry->data = data;
    entry->next = table->buckets[index];
    table->buckets[index] = entry;
    
    pthread_mutex_unlock(&table->lock);
    printf("Data inserted into hash table (O(1)): %s\n", key);
}

void* hash_get(HashTable* table, const char* key, int user_id) {
    pthread_mutex_lock(&table->lock);
    
    unsigned int index = hash_function(key);
    HashEntry* entry = table->buckets[index];
    
    while (entry != NULL) {
        if (entry->user_id == user_id && my_strcmp(entry->key, key) == 0) {
            void* data = entry->data;
            pthread_mutex_unlock(&table->lock);
            printf("Data retrieved from hash table (O(1)): %s\n", key);
            return data;
        }
        entry = entry->next;
    }
    
    pthread_mutex_unlock(&table->lock);
    return NULL;
}

void hash_delete(HashTable* table, const char* key, int user_id) {
    pthread_mutex_lock(&table->lock);
    
    unsigned int index = hash_function(key);
    HashEntry* entry = table->buckets[index];
    HashEntry* prev = NULL;
    
    while (entry != NULL) {
        if (entry->user_id == user_id && my_strcmp(entry->key, key) == 0) {
            if (prev == NULL) {
                table->buckets[index] = entry->next;
            } else {
                prev->next = entry->next;
            }
            free(entry->data);
            free(entry);
            pthread_mutex_unlock(&table->lock);
            printf("Data deleted from hash table (O(1)): %s\n", key);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
    
    pthread_mutex_unlock(&table->lock);
}

void hash_destroy(HashTable* table) {
    if (table) {
        for (int i = 0; i < MAX_HASH_SIZE; i++) {
            HashEntry* entry = table->buckets[i];
            while (entry != NULL) {
                HashEntry* next = entry->next;
                free(entry->data);
                free(entry);
                entry = next;
            }
        }
        pthread_mutex_destroy(&table->lock);
        free(table);
    }
}
