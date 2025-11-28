#include "globals.h"

int add_journal(int user_id, const char* entry, const char* date) {
    const char* sql = "INSERT INTO journals (user_id, entry, date, created_at) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, entry, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, date, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 4, time(NULL));
    
    int rc = sqlite3_step(stmt);
    int journal_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to add journal: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("Journal added successfully (ID: %d)\n", journal_id);
    
    if (!is_online) {
        char data[MAX_STRING];
        sprintf(data, "%d|%s|%s", user_id, entry, date);
        sync_enqueue(sync_queue, "INSERT", "journals", data);
    }
    
    return journal_id;
}

int get_user_journals(int user_id, Journal** journals, int* count) {
    const char* sql = "SELECT id, user_id, entry, date, created_at FROM journals WHERE user_id = ? ORDER BY created_at DESC;";
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
    
    *journals = (Journal*)malloc((*count) * sizeof(Journal));
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*journals)[i].id = sqlite3_column_int(stmt, 0);
        (*journals)[i].user_id = sqlite3_column_int(stmt, 1);
        my_strcpy((*journals)[i].entry, (const char*)sqlite3_column_text(stmt, 2));
        my_strcpy((*journals)[i].date, (const char*)sqlite3_column_text(stmt, 3));
        (*journals)[i].created_at = sqlite3_column_int64(stmt, 4);
        i++;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

int get_journals_by_date(int user_id, const char* date, Journal** journals, int* count) {
    const char* sql
    = "SELECT id, user_id, entry, date, created_at FROM journals WHERE user_id = ? AND date = ?;";
sqlite3_stmt* stmt;
if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    return -1;
}

sqlite3_bind_int(stmt, 1, user_id);
sqlite3_bind_text(stmt, 2, date, -1, SQLITE_STATIC);

*count = 0;
while (sqlite3_step(stmt) == SQLITE_ROW) {
    (*count)++;
}

sqlite3_reset(stmt);

*journals = (Journal*)malloc((*count) * sizeof(Journal));

int i = 0;
while (sqlite3_step(stmt) == SQLITE_ROW) {
    (*journals)[i].id = sqlite3_column_int(stmt, 0);
    (*journals)[i].user_id = sqlite3_column_int(stmt, 1);
    my_strcpy((*journals)[i].entry, (const char*)sqlite3_column_text(stmt, 2));
    my_strcpy((*journals)[i].date, (const char*)sqlite3_column_text(stmt, 3));
    (*journals)[i].created_at = sqlite3_column_int64(stmt, 4);
    i++;
}

sqlite3_finalize(stmt);
return 0;
}
