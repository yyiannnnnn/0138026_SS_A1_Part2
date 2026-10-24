#include "header.h"

/*
./FileSystem -i   create a user
./FileSystem      log in and use the file system
*/
int main(int argc, char **argv)
{
	char hex[33];
	FILE *fp;

	srand((unsigned int)time(NULL) ^ ((unsigned int)getpid() << 16));

	/* required MD5 self test (computed, not hard coded) */
	md5_string("This is a test", hex);
	printf("MD5 (\"This is a test\") = %s\n", hex);

	if (argc > 1 && strcmp(argv[1], "-i") == 0)
	{
		account_creation();
		return (0);
	}
	fp = fopen("shadow.txt", "r");
	if (fp == NULL || access("salt.txt", F_OK) != 0)
	{
		printf("salt.txt / shadow.txt not found. Run ./FileSystem -i first.\n");
		if (fp)
			fclose(fp);
		return (1);
	}
	fclose(fp);
	logging_in();
	return (0);
}
