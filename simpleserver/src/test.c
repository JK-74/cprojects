#include <stdlib.h>
#include <stdio.h>
#include "networking.h"

int main(int argc, char** argv[])
{
	SHA256_CTX ctx;
	BYTE hash[SHA256_BLOCK_SIZE];
	FILE* fp;
	char* creds = "Sks0NzpwYXNzd29yZA==";
	sha256_init(&ctx);
	sha256_update(&ctx, creds, strlen(creds));
	sha256_final(&ctx, hash);
	if((fp = fopen("userhash.dat", "w+")) == NULL)
		exit(0);
	fwrite(hash, sizeof(char), sizeof(hash), fp);
	fclose(fp);
	for(int i = 0; i < 32; i++)
	{
		printf("%x", hash[i]);
	}
}
