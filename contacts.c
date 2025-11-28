
#include "globals.h"

int add_contact(int user_id, const char* name, const char* relationship, const char* phone) {
    const char* sql = "INSERT INTO contacts (user_id, name, relationship, phone) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, relationship, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, phone, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    int contact_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to add contact: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("Contact added successfully (ID: %d)\n", contact_id);
    
    if (!is_online) {
        char data[MAX_STRING];
        sprintf(data, "%d|%s|%s|%s", user_id, name, relationship, phone);
        sync_enqueue(sync_queue, "INSERT", "contacts", data);
    }
    
    return contact_id;
}

int get_user_contacts(int user_id, Contact** contacts, int* count) {
    const char* sql = "SELECT id, user_id, name, relationship, phone FROM contacts WHERE user_id = ?;";
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
    
    *contacts = (Contact*)malloc((*count) * sizeof(Contact));
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*contacts)[i].id = sqlite3_column_int(stmt, 0);
        (*contacts)[i].user_id = sqlite3_column_int(stmt, 1);
        my_strcpy((*contacts)[i].name, (const char*)sqlite3_column_text(stmt, 2));
        my_strcpy((*contacts)[i].relationship, (const char*)sqlite3_column_text(stmt, 3));
        my_strcpy((*contacts)[i].phone, (const char*)sqlite3_column_text(stmt, 4));
        i++;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

int delete_contact(int contact_id) {
    const char* sql = "DELETE FROM contacts WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, contact_id);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to delete contact: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("Contact deleted (ID: %d)\n", contact_id);
    
    if (!is_online) {
        char data[MAX_STRING];
        sprintf(data, "%d", contact_id);
        sync_enqueue(sync_queue, "DELETE", "contacts", data);
    }
    
    return 0;
}
