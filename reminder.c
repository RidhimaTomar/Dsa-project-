#include "globals.h"
#include "reminder.h"
#include <string.h>

ReminderNode reminderQueue[MAX_REMINDERS];
int reminderCount = 0;
HashNode hashTable[HASH_SIZE];
int reminderIdCounter = 1;

int hashTime(time_t t) {
    return (int)(t % HASH_SIZE);
}

void initHashTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].count = 0;
        for (int j = 0; j < MAX_REMINDERS; j++)
            hashTable[i].reminders[j] = NULL;
    }
}

// Heap functions
void heapifyUp(int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (reminderQueue[idx].priority > reminderQueue[parent].priority ||
            (reminderQueue[idx].priority == reminderQueue[parent].priority &&
             reminderQueue[idx].timestamp < reminderQueue[parent].timestamp)) {
            ReminderNode temp = reminderQueue[idx];
            reminderQueue[idx] = reminderQueue[parent];
            reminderQueue[parent] = temp;
            idx = parent;
        } else break;
    }
}

void heapifyDown(int idx) {
    while (1) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int highest = idx;

        if (left < reminderCount &&
            (reminderQueue[left].priority > reminderQueue[highest].priority ||
             (reminderQueue[left].priority == reminderQueue[highest].priority &&
              reminderQueue[left].timestamp < reminderQueue[highest].timestamp))) {
            highest = left;
        }

        if (right < reminderCount &&
            (reminderQueue[right].priority > reminderQueue[highest].priority ||
             (reminderQueue[right].priority == reminderQueue[highest].priority &&
              reminderQueue[right].timestamp < reminderQueue[highest].timestamp))) {
            highest = right;
        }

        if (highest != idx) {
            ReminderNode temp = reminderQueue[idx];
            reminderQueue[idx] = reminderQueue[highest];
            reminderQueue[highest] = temp;
            idx = highest;
        } else break;
    }
}

void addReminder(char* text, time_t timestamp, int priority, int userId) {
    if (reminderCount >= MAX_REMINDERS) return;
    
    reminderQueue[reminderCount].id = reminderIdCounter++;
    strcpy(reminderQueue[reminderCount].text, text);
    reminderQueue[reminderCount].timestamp = timestamp;
    reminderQueue[reminderCount].priority = priority;
    reminderQueue[reminderCount].userId = userId;
    
    // Add to hash table
    int hashIdx = hashTime(timestamp);
    hashTable[hashIdx].reminders[hashTable[hashIdx].count++] = &reminderQueue[reminderCount];
    
    heapifyUp(reminderCount);
    reminderCount++;
}
void removeTopReminder() {
    if (reminderCount == 0) return;
    reminderQueue[0] = reminderQueue[reminderCount - 1];
    reminderCount--;
    if (reminderCount > 0) heapifyDown(0);
}
