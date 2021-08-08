#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sha256.h"

#define BUFFER_LEN 1024
#define PORT 7890

#define RESPONSE_TEMPLATE "HTTP/1.1 %d %s\r\nTransfer-Encoding: chunked\r\nServer: JK-47_HTTP 1.0\r\nAccept-Ranges: bytes\r\nContent-Type: text/html\r\n\r\n"

typedef enum SocketFlags
{
	SUCCESS,
	SOCKET_FAILURE, 
	BIND_FAILURE, 
	LISTEN_FAILURE
} SocketFlags;

typedef enum HttpMethods
{
	GET = 1,
	POST,
	PUT,
	DELETE,
	OPTIONS,
	HEAD,
} HttpMethods;

int SocketInit(struct sockaddr_in* hostaddr, int* fd);

void* SendResponse(FILE* fp, int socket, char* header);

void ReceiveMessage(char* buffer, int* fd);

int GetFileSize(FILE* fp);

int StartsWith(char* str, char* target);

HttpMethods HttpResolveMethod(char* payload);

char* HttpParse(char* buffer, char delimiter);

char* HttpBuildHeader(int statCode, char* statString);

bool HttpAuth(char* buffer);

void DumpBuffer(char* buffer);
