#include "globals.h"

MaxHeap* create_max_heap() {
    MaxHeap* heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    heap->size = 0;
    pthread_mutex_init(&heap->lock, NULL);
    return heap;
}

void heap_heapify_up(MaxHeap* heap, int index) {
    if (index == 0) return;
    
    int parent = (index - 1) / 2;
    
    if (heap->reminders[index]->priority > heap->reminders[parent]->priority) {
        Reminder* temp = heap->reminders[index];
        heap->reminders[index] = heap->reminders[parent];
        heap->reminders[parent] = temp;
        heap_heapify_up(heap, parent);
    }
}

void heap_heapify_down(MaxHeap* heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    
    if (left < heap->size && 
        heap->reminders[left]->priority > heap->reminders[largest]->priority) {
        largest = left;
    }
    
    if (right < heap->size && 
        heap->reminders[right]->priority > heap->reminders[largest]->priority) {
        largest = right;
    }
    
    if (largest != index) {
        Reminder* temp = heap->reminders[index];
        heap->reminders[index] = heap->reminders[largest];
        heap->reminders[largest] = temp;
        heap_heapify_down(heap, largest);
    }
}

void heap_insert(MaxHeap* heap, Reminder* reminder) {
    pthread_mutex_lock(&heap->lock);
    
    if (heap->size >= MAX_REMINDERS) {
        pthread_mutex_unlock(&heap->lock);
        return;
    }
    
    heap->reminders[heap->size] = reminder;
    heap_heapify_up(heap, heap->size);
    heap->size++;
    
    pthread_mutex_unlock(&heap->lock);
    printf("Reminder inserted into heap (O(log n)). Heap size: %d\n", heap->size);
}

Reminder* heap_extract_max(MaxHeap* heap) {
    pthread_mutex_lock(&heap->lock);
    
    if (heap->size == 0) {
        pthread_mutex_unlock(&heap->lock);
        return NULL;
    }
    
    Reminder* max = heap->reminders[0];
    heap->reminders[0] = heap->reminders[heap->size - 1];
    heap->size--;
    heap_heapify_down(heap, 0);
    
    pthread_mutex_unlock(&heap->lock);
    return max;
}

void heap_destroy(MaxHeap* heap) {
    if (heap) {
        for (int i = 0; i < heap->size; i++) {
            free(heap->reminders[i]);
        }
        pthread_mutex_destroy(&heap->lock);
        free(heap);
    }
}
