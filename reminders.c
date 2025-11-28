#include "globals.h"

int add_reminder(int user_id, const char* text, time_t reminder_time, int priority) {
    const char* sql = "INSERT INTO reminders (user_id, text, reminder_time, priority, notified) VALUES (?, ?, ?, ?, 0);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, text, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, reminder_time);
    sqlite3_bind_int(stmt, 4, priority);
    
    int rc = sqlite3_step(stmt);
    int reminder_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to add reminder: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    Reminder* reminder = (Reminder*)malloc(sizeof(Reminder));
    reminder->id = reminder_id;
    reminder->user_id = user_id;
    my_strncpy(reminder->text, text, MAX_STRING - 1);
    reminder->text[MAX_STRING - 1] = '\0';
    reminder->reminder_time = reminder_time;
    reminder->priority = priority;
    reminder->notified = 0;
    
    heap_insert(reminder_heap, reminder);
    
    char key[MAX_STRING];
    sprintf(key, "reminder_%d", reminder_id);
    hash_insert(user_data_table, key, reminder, user_id);
    
    printf("Reminder added successfully (ID: %d)\n", reminder_id);
    
    if (!is_online) {
        char data[MAX_STRING];
        sprintf(data, "%d|%s|%ld|%d", user_id, text, reminder_time, priority);
        sync_enqueue(sync_queue, "INSERT", "reminders", data);
    }
    
    return reminder_id;
}

int get_user_reminders(int user_id, Reminder** reminders, int* count) {
    const char* sql = "SELECT id, user_id, text, reminder_time, priority, notified FROM reminders WHERE user_id = ? AND notified = 0;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    *count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*count)++;
    }
    
    sqlite3_reset(stmt);
    
    *reminders = (Reminder*)malloc((*count) * sizeof(Reminder));
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*reminders)[i].id = sqlite3_column_int(stmt, 0);
        (*reminders)[i].user_id = sqlite3_column_int(stmt, 1);
        my_strcpy((*reminders)[i].text, (const char*)sqlite3_column_text(stmt, 2));
        (*reminders)[i].reminder_time = sqlite3_column_int64(stmt, 3);
        (*reminders)[i].priority = sqlite3_column_int(stmt, 4);
        (*reminders)[i].notified = sqlite3_column_int(stmt, 5);
        i++;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

int delete_reminder(int reminder_id) {
    const char* sql = "DELETE FROM reminders WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, reminder_id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to delete reminder: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("Reminder deleted (ID: %d)\n", reminder_id);
    
    if (!is_online) {
        char data[MAX_STRING];
        sprintf(data, "%d", reminder_id);
        sync_enqueue(sync_queue, "DELETE", "reminders", data);
    }
    
    return 0;
}

int update_reminder_notified(int reminder_id) {
    const char* sql = "UPDATE reminders SET notified = 1 WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, reminder_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return 0;
}
