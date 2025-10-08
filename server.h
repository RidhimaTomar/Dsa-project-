#ifndef SERVER_H
#define SERVER_H

void sendResponse(int client, char* status, char* contentType, char* body);
void handleRequest(int client, char* request);

#endif
