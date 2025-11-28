#include "globals.h"

int init_database() {
    int rc = sqlite3_open("alzheimers_app.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("Database opened successfully\n");
    return 0;
}

void close_database() {
    if (db) {
        sqlite3_close(db);
        printf("Database closed\n");
    }
}

int create_tables() {
    char* err_msg = NULL;
    
    const char* sql_users = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "user_code TEXT UNIQUE NOT NULL,"
        "created_at INTEGER);";
    
    const char* sql_reminders = 
        "CREATE TABLE IF NOT EXISTS reminders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "text TEXT NOT NULL,"
        "reminder_time INTEGER NOT NULL,"
        "priority INTEGER DEFAULT 1,"
        "notified INTEGER DEFAULT 0,"
        "FOREIGN KEY (user_id) REFERENCES users(id));";
    
    const char* sql_journals = 
        "CREATE TABLE IF NOT EXISTS journals ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "entry TEXT NOT NULL,"
        "date TEXT NOT NULL,"
        "created_at INTEGER,"
        "FOREIGN KEY (user_id) REFERENCES users(id));";
    
    const char* sql_moods = 
        "CREATE TABLE IF NOT EXISTS moods ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "mood TEXT NOT NULL,"
        "date TEXT NOT NULL,"
        "created_at INTEGER,"
        "FOREIGN KEY (user_id) REFERENCES users(id));";
    
    const char* sql_contacts = 
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "name TEXT NOT NULL,"
        "relationship TEXT NOT NULL,"
        "phone TEXT NOT NULL,"
        "FOREIGN KEY (user_id) REFERENCES users(id));";
    
    // Create all tables
    if (sqlite3_exec(db, sql_users, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    if (sqlite3_exec(db, sql_reminders, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    if (sqlite3_exec(db, sql_journals, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    if (sqlite3_exec(db, sql_moods, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    if (sqlite3_exec(db, sql_contacts, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    printf("All tables created successfully\n");
    return 0;
}
