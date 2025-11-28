#include "globals.h"

SyncQueue* create_sync_queue() {
    SyncQueue* queue = (SyncQueue*)malloc(sizeof(SyncQueue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->lock, NULL);
    printf("Sync queue created for offline-first architecture\n");
    return queue;
}

void sync_enqueue(SyncQueue* queue, const char* operation, const char* table, const char* data) {
    pthread_mutex_lock(&queue->lock);
    
    SyncNode* node = (SyncNode*)malloc(sizeof(SyncNode));
    my_strncpy(node->operation, operation, 31);
    node->operation[31] = '\0';
    my_strncpy(node->table, table, 63);
    node->table[63] = '\0';
    my_strncpy(node->data, data, MAX_STRING - 1);
    node->data[MAX_STRING - 1] = '\0';
    node->timestamp = time(NULL);
    node->next = NULL;
    
    if (queue->tail == NULL) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    
    queue->size++;
    
    pthread_mutex_unlock(&queue->lock);
    printf("Operation queued for sync: %s on %s (Queue size: %d)\n", 
           operation, table, queue->size);
}

SyncNode* sync_dequeue(SyncQueue* queue) {
    pthread_mutex_lock(&queue->lock);
    
    if (queue->head == NULL) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }
    
    SyncNode* node = queue->head;
    queue->head = queue->head->next;
    
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    
    queue->size--;
    
    pthread_mutex_unlock(&queue->lock);
    return node;
}

void sync_process_queue(SyncQueue* queue) {
    if (!is_online) {
        printf("System offline - operations queued for later sync\n");
        return;
    }
    
    printf("Processing sync queue...\n");
    
    SyncNode* node;
    while ((node = sync_dequeue(queue)) != NULL) {
        printf("Syncing: %s operation on %s\n", node->operation, node->table);
        free(node);
    }
    
    printf("Sync queue processed\n");
}

void sync_destroy(SyncQueue* queue) {
    if (queue) {
        SyncNode* node = queue->head;
        while (node != NULL) {
            SyncNode* next = node->next;
            free(node);
            node = next;
        }
        pthread_mutex_destroy(&queue->lock);
        free(queue);
    }
}
