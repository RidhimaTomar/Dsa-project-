
#include "globals.h"

void check_and_notify_reminders() {
    time_t current_time = time(NULL);
    
    const char* sql = "SELECT id, user_id, text, reminder_time FROM reminders WHERE notified = 0 AND reminder_time <= ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return;
    }
    
    sqlite3_bind_int64(stmt, 1, current_time);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int reminder_id = sqlite3_column_int(stmt, 0);
        int user_id = sqlite3_column_int(stmt, 1);
        const char* text = (const char*)sqlite3_column_text(stmt, 2);
        
        printf("\n========================================\n");
        printf("REMINDER NOTIFICATION!\n");
        printf("User ID: %d\n", user_id);
        printf("Message: %s\n", text);
        printf("========================================\n");
        
        // Play sound notification (10 seconds)
        printf("Playing sound notification for 10 seconds...\n");
        
        // Update as notified
        update_reminder_notified(reminder_id);
    }
    
    sqlite3_finalize(stmt);
}

void* notification_thread(void* arg) {
    printf("Notification thread started - checking every %d seconds\n", NOTIFICATION_INTERVAL);
    
    while (1) {
        check_and_notify_reminders();
        sleep(NOTIFICATION_INTERVAL);
    }
    
    return NULL;
}

void start_notification_scheduler() {
    pthread_t thread_id;
    
    if (pthread_create(&thread_id, NULL, notification_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create notification thread\n");
        return;
    }
    
    pthread_detach(thread_id);
    printf("Multi-threaded notification scheduler started\n");
}
