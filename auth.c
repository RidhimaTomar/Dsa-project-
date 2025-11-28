#include "globals.h"

void sha256_hash(const char* password, char* output_hash) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password, my_strlen(password), hash);
    
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output_hash + (i * 2), "%02x", hash[i]);
    }
    output_hash[64] = '\0';
}

void generate_user_code(char* code) {
    sprintf(code, "ALZ%06d", rand() % 1000000);
}

int register_user(const char* username, const char* password, char* user_code) {
    char password_hash[65];
    sha256_hash(password, password_hash);
    
    char code[16];
    generate_user_code(code);
    my_strcpy(user_code, code);
    
    const char* sql = "INSERT INTO users (username, password_hash, user_code, created_at) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password_hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, code, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 4, time(NULL));
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Registration failed: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("User registered successfully with code: %s\n", code);
    
    if (!is_online) {
        char data[MAX_STRING];
        my_strcpy(data, username);
        my_strcat(data, "|");
        my_strcat(data, password_hash);
        my_strcat(data, "|");
        my_strcat(data, code);
        sync_enqueue(sync_queue, "INSERT", "users", data);
    }
    
    return 0;
}

int login_user(const char* username, const char* password, User* user) {
    char password_hash[65];
    sha256_hash(password, password_hash);
    
    const char* sql = "SELECT id, username, password_hash, user_code, created_at FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        const char* stored_hash = (const char*)sqlite3_column_text(stmt, 2);
        
        if (my_strcmp(password_hash, stored_hash) == 0) {
            user->id = sqlite3_column_int(stmt, 0);
            my_strcpy(user->username, (const char*)sqlite3_column_text(stmt, 1));
            my_strcpy(user->password_hash, stored_hash);
            my_strcpy(user->user_code, (const char*)sqlite3_column_text(stmt, 3));
            user->created_at = sqlite3_column_int64(stmt, 4);
            
            sqlite3_finalize(stmt);
            printf("Login successful for user: %s (Code: %s)\n", user->username, user->user_code);
            return 0;
        }
    }
    
    sqlite3_finalize(stmt);
    fprintf(stderr, "Login failed: Invalid credentials\n");
    return -1;
}
