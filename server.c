#include "globals.h"
#include "server.h"
#include "user.h"
#include "notes.h"
#include "reminder.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void sendResponse(int client, char* status, char* contentType, char* body) { 
    char response[8192];
    sprintf(response,
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n%s",
        status, contentType, strlen(body), body);
    send(client, response, strlen(response), 0);
}
void handleRequest(int client, char* request) { char method[10], path[100];
    sscanf(request, "%s %s", method, path);
    
    if (strcmp(method, "POST") == 0 && strcmp(path, "/api/login") == 0) {
        char username[50], password[50];
        char* body = strstr(request, "\r\n\r\n") + 4;
        sscanf(body, "{\"username\":\"%[^\"]\",\"password\":\"%[^\"]\"}", username, password);
        
        int userId = authenticateUser(username, password);
        char response[256];
        if (userId != -1) {
            sprintf(response, "{\"success\":true,\"userId\":%d}", userId);
            sendResponse(client, "200 OK", "application/json", response);
        } else {
            sprintf(response, "{\"success\":false,\"message\":\"Invalid credentials\"}");
            sendResponse(client, "401 Unauthorized", "application/json", response);
        }
    }
}