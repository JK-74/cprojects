#include "networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char **argv[])
{
	int sockfd, connfd, queryfd;
	struct sockaddr_in hostaddr, cliaddr;
	socklen_t size = sizeof(struct sockaddr_in);
	SocketFlags flag;
	flag = SocketInit(&hostaddr, &sockfd);
	char* recBuffer = calloc(BUFFER_LEN, sizeof(char));
	char* filename;
	FILE* fp = NULL;
	switch(flag)
	{
		case SOCKET_FAILURE:
			printf("failed to fetch socket file descriptor\n");
			break;
		
		case BIND_FAILURE:
			printf("failed to bind socket to local address\n");
			break;

		case LISTEN_FAILURE:
			printf("failed to listen on port %d\n", PORT);
			break;
		default:
			printf("Successfully initialized socket. Listening on port %d.\n", PORT);
	}
	if(flag != SUCCESS)
	{
		perror("error message: ");
		exit(0);
	}
	//connection loop
	while(true)
	{
		if((connfd = accept(sockfd, (struct sockaddr*) &cliaddr, &size)) == -1)		
		{
			perror("error message: ");
			exit(0);
		}
		memset(recBuffer, '\0', BUFFER_LEN);
		ReceiveMessage(recBuffer, &connfd);
		DumpBuffer(recBuffer);
		if(!HttpAuth(recBuffer))
		{
			SendResponse(fp, connfd, HttpBuildHeader(401, "Unauthorized"));
		}
		else
		{
			flag = HttpResolveMethod(recBuffer);
			recBuffer = strstr(recBuffer, "/"); //seek beginning of queried filepath
			filename = HttpParse(recBuffer + 1, ' '); //note: allocates memory;
			switch(flag)
			{
				case GET:
					printf("filename is %s\n", filename);
					if((fp = fopen(filename, "rb")) == NULL)
						SendResponse(fp, connfd, HttpBuildHeader(404, "NOT FOUND"));
					else
						SendResponse(fp, connfd, HttpBuildHeader(200,"OK"));
					free(filename);
					break;
				case POST:
					printf("Post request detected");
					break;
				case PUT:
					printf("Put request detected");
					break;
				case DELETE:
					printf("Delete request detected");
					break;
				case OPTIONS:
					printf("Options request detected");
					break;
				case HEAD:
					printf("Head request detected");
					break;
				default:
					printf("No HTTP method detected");
					break;
			}
			fclose(fp);
			fp = NULL;
		}
		fflush(stdin);
		close(connfd);

	}


}
