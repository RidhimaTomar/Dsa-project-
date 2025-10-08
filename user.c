#include "globals.h"
#include "user.h"
#include <string.h>

User users[MAX_USERS];
int userCount = 0;

int authenticateUser(char* username, char* password) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            return users[i].id;
        }
    }
    return -1;
}

int registerUser(char* username, char* password) {
    if (userCount >= MAX_USERS) return -1;
    for (int i = 0; i < userCount; i++)
        if (strcmp(users[i].username, username) == 0) return -1;

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].id = userCount;
    userCount++;
    return userCount - 1;
}