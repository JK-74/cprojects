#include "networking.h"
#include <errno.h>
#include <stdbool.h>
int SocketInit(struct sockaddr_in* hostaddr, int* fd)
{
	hostaddr->sin_port = htons(PORT);
	hostaddr->sin_family = AF_INET;
	hostaddr->sin_addr.s_addr = 0;
	socklen_t size = sizeof(struct sockaddr_in);
	SocketFlags flag = SUCCESS;
	if((*fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		return SOCKET_FAILURE;
	}
	if(bind(*fd, (struct sockaddr*)hostaddr, size) == -1)
	{
		return BIND_FAILURE;
	}
	if(listen(*fd, 20) == -1)
	{
		return LISTEN_FAILURE;
	}
	return SUCCESS;
}

void* SendResponse(FILE* fp, int socket, char* header)
{
	char chunk[BUFFER_LEN];
	size_t readBytes = 0;
	char chunkLength[BUFFER_LEN];
	send(socket, header, strlen(header), 0);
	if(fp != NULL)
	{
		while(true)
		{
			readBytes = fread(chunk, sizeof(char), BUFFER_LEN, fp);	
			sprintf(chunkLength,"%x\r\n", (unsigned int) readBytes);
			send(socket, chunkLength, strlen(chunkLength), 0);
			if(readBytes != 0)
			{
				send(socket, chunk, readBytes, 0);
				send(socket, "\r\n", 2, 0);
			}
			else
				break;
			
		}
		send(socket, "\r\n", 2, 0);
	}
	else
	{
		send(socket, "0\r\n\r\n", 5, 0);
	}

	
}

void ReceiveMessage(char* buffer, int* fd)
{
	int len = recv(*fd, buffer, (size_t) BUFFER_LEN, 0);
	/*while(len > 0)
	{
		(len = recv(*fd, buffer, (size_t) BUFFER_LEN, 0));
	}
	*/
}


int StartsWith(char* str, char* target)
{
	if (!memcmp(str, target, strlen(target)))
	{
		return true;
	}
	return false;
}

HttpMethods HttpResolveMethod(char* payload)
{
	if(StartsWith(payload, "GET "))
	{
	       return GET;
	}
	else if(StartsWith(payload, "POST "))
	{
		return POST;
	}
	else if(StartsWith(payload, "PUT "))
	{
		return PUT;
	}
	else if (StartsWith(payload, "DELETE "))
	{
		return DELETE;
	}
	else if (StartsWith(payload, "OPTIONS "))
	{
		return OPTIONS;
	}
	else if (StartsWith(payload, "HEAD "))
	{
		return HEAD;
	}
	return -1;
}

char* HttpParse(char* payload, char delimiter)
{
	char* filename = calloc(BUFFER_LEN, sizeof(char));
	for(int i = 0; payload[i] != delimiter; i++)
	{
		filename[i] = payload[i];
	}
	return filename;
}

char* HttpBuildHeader(int statCode, char* statString)
{
	char* buffer = calloc(BUFFER_LEN, sizeof(char));
	sprintf(buffer, RESPONSE_TEMPLATE, statCode, statString);
	return buffer;
}

bool HttpAuth(char* buffer)
{
	char* ptr;
	char* creds;
	FILE* fp;
	char* target = "Authorization: Basic ";
	char* userhash[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;
	BYTE hash[SHA256_BLOCK_SIZE];
	if(!(buffer = strstr(buffer, target)))
	{
		return false;
	}
	buffer += strlen(target);
	creds = HttpParse(buffer, '\r');
	sha256_init(&ctx);
	sha256_update(&ctx, creds, strlen(creds));
	sha256_final(&ctx, hash);
	if((fp = fopen("userhash.dat", "rb")) != NULL)
	{
		fread(userhash, sizeof(char), SHA256_BLOCK_SIZE, fp);
		if(!memcmp(userhash, hash, SHA256_BLOCK_SIZE))
		{
			fclose(fp);
			free(creds);
			return true;
		}
	}
	free(creds);
	return false;
}
int GetFileSize(FILE* fp)
{
	size_t dump = 0;
	int size = 0;
	void* buffer[BUFFER_LEN];
	while(true)
	{
		dump = fread(buffer, sizeof(char), BUFFER_LEN, fp);
		size += dump;
		if(!dump)
			break;		
	}
	fseek(fp, 0, SEEK_SET);
	return size;
}

void DumpBuffer(char* buffer)
{
	for(int i = 0; buffer[i] != (char*) NULL; i++)
	{	
		if(i % 4 == 0)
		{
			printf("\n");
		}
		
		printf("%2x ", buffer[i]);
	}
	printf("\n");
}


