#include "header.h"

/* looks the user up in salt.txt; returns 1 and fills salt_out if found */
int get_user_salt(char *username, char *salt_out)
{
	FILE *fp = fopen("salt.txt", "r");
	char line[256], stored_username[USER_LEN], stored_salt[9];

	if (fp == NULL)
		return (0);
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (sscanf(line, "%20[^:]:%8[^\n]", stored_username, stored_salt) == 2
			&& strcmp(stored_username, username) == 0)
		{
			strcpy(salt_out, stored_salt);
			fclose(fp);
			return (1);
		}
	}
	fclose(fp);
	return (0);
}

/* looks the user up in shadow.txt; returns 1 and fills hash/clearance if found */
int get_user_shadow(char *username, char *hash_out, int *clearance_out)
{
	FILE *fp = fopen("shadow.txt", "r");
	char line[256], stored_username[USER_LEN], stored_hash[33];
	int stored_clearance;

	if (fp == NULL)
		return (0);
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (sscanf(line, "%20[^:]:%32[^:]:%d", stored_username, stored_hash, &stored_clearance) == 3
			&& strcmp(stored_username, username) == 0)
		{
			strcpy(hash_out, stored_hash);
			*clearance_out = stored_clearance;
			fclose(fp);
			return (1);
		}
	}
	fclose(fp);
	return (0);
}

/* login mode: authenticate, then hand over to the file menu */
void logging_in(void)
{
	char username[USER_LEN], password[PW_LEN];
	char salt[9], computed_hash[33], stored_hash[33];
	int clearance;

	read_line("Username: ", username, sizeof(username));
	read_password("Password: ", password, sizeof(password));

	if (!get_user_salt(username, salt))
	{
		printf("Login failed: %s not found in salt.txt.\n", username);
		exit(1);
	}
	printf("%s found in salt.txt\n", username);
	printf("salt retrieved: %s\n", salt);
	printf("hashing ...\n");
	create_hash(password, salt, computed_hash);
	printf("hash value: %s\n", computed_hash);

	if (!get_user_shadow(username, stored_hash, &clearance))
	{
		printf("Authentication failed: %s not found in shadow.txt.\n", username);
		exit(1);
	}
	if (strcmp(computed_hash, stored_hash) != 0)
	{
		printf("Authentication failed: incorrect password.\n");
		exit(1);
	}
	printf("Authentication for user %s complete.\n", username);
	printf("The clearance for %s is %d.\n", username, clearance);
	file_menu_loop(clearance, username);
}
