#include "globals.h"

sqlite3* db = NULL;
MaxHeap* reminder_heap = NULL;
HashTable* user_data_table = NULL;
SyncQueue* sync_queue = NULL;
int is_online = 1;

int main() {
    printf("=== Alzheimer's Memory Management System ===\n");
    printf("Initializing system...\n");
    
    // Initialize database
    if (init_database() != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }
    
    // Create tables
    if (create_tables() != 0) {
        fprintf(stderr, "Failed to create tables\n");
        close_database();
        return 1;
    }
    
    // Initialize data structures
    reminder_heap = create_max_heap();
    user_data_table = create_hash_table();
    sync_queue = create_sync_queue();
    
    printf("System initialized successfully!\n");
    printf("Max Heap created for O(log n) priority-based reminders\n");
    printf("Hash Table created for O(1) time-based retrieval\n");
    printf("Sync Queue created for offline-first architecture\n");
    
    // Start notification scheduler thread
    start_notification_scheduler();
    
    // Keep main thread running
    printf("\nNotification scheduler running (checks every 30 seconds)\n");
    printf("Press Ctrl+C to exit\n\n");
    
    while (1) {
        sleep(60);
    }
    
    // Cleanup
    heap_destroy(reminder_heap);
    hash_destroy(user_data_table);
    sync_destroy(sync_queue);
    close_database();
    
    return 0;
}
