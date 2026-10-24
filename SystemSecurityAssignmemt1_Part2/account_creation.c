#include "header.h"

/* makes sure the file exists and ends with a newline before appending */
static void ensure_newline_before_append(const char *filename)
{
	FILE *fp = fopen(filename, "a+");
	long size;

	if (fp == NULL)
	{
		perror(filename);
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	if (size > 0)
	{
		fseek(fp, -1, SEEK_END);
		if (fgetc(fp) != '\n')
			fputc('\n', fp);
	}
	fclose(fp);
}

/*
-i mode: create a user.
salt.txt   -> username:salt
shadow.txt -> username:MD5(password+salt):clearance
*/
void account_creation(void)
{
	char username[USER_LEN];
	char password[PW_LEN];
	char confirm[PW_LEN];
	char buf[16];
	char salt[9];
	char hash[33];
	int  clearance_level;
	int  attempts_left = 3;
	FILE *fp;

	printf("Creating a user (press enter after input)\n");
	read_line("Enter username (max length 20 characters): ", username, sizeof(username));
	if (username[0] == '\0' || strpbrk(username, ": \t") != NULL)
	{
		printf("Invalid username (no spaces or ':'). Aborting...\n");
		exit(1);
	}
	if (check_existing_user(username) != 0)
	{
		printf("This user already exists. Aborting...\n");
		exit(0);
	}

	printf("Password must adhere to the following policy:\n"
		"1. Minimum length: 12 characters\n"
		"2. Must contain at least:\n"
		"   - 1 lowercase letter (a-z)\n"
		"   - 1 uppercase letter (A-Z)\n"
		"   - 1 digit (0-9)\n"
		"   - 1 special character from: !@#$%%^&*()-_=+\n"
		"3. Password cannot equal the username\n"
		"4. Password and confirmation must match\n");

	while (1)
	{
		read_password("Enter password (max length 64 characters): ", password, sizeof(password));
		read_password("Confirm password: ", confirm, sizeof(confirm));
		if (strcmp(password, confirm) != 0)
			printf("Passwords do not match.\n");
		else if (policy_check(password, username) == 0)
			break;
		if (--attempts_left <= 0)
		{
			printf("No attempts left. Aborting...\n");
			exit(1);
		}
		printf("%d attempt(s) remaining.\n\n", attempts_left);
	}

	while (1)
	{
		read_line("Select user clearance level (0, 1, 2 or 3): ", buf, sizeof(buf));
		if (strlen(buf) == 1 && buf[0] >= '0' && buf[0] <= '3')
		{
			clearance_level = buf[0] - '0';
			break;
		}
		printf("Invalid clearance level.\n");
	}

	create_salt(salt);
	create_hash(password, salt, hash);

	ensure_newline_before_append("salt.txt");
	fp = fopen("salt.txt", "a");
	if (fp == NULL) { perror("fopen salt.txt"); exit(1); }
	fprintf(fp, "%s:%s\n", username, salt);
	fclose(fp);

	ensure_newline_before_append("shadow.txt");
	fp = fopen("shadow.txt", "a");
	if (fp == NULL) { perror("fopen shadow.txt"); exit(1); }
	fprintf(fp, "%s:%s:%d\n", username, hash, clearance_level);
	fclose(fp);

	printf("User %s created successfully.\n", username);
}

/*
Password policy (listed in full when something is missing).
Returns 0 if the password is acceptable, 1 otherwise.

Entropy (all 76 symbols equally likely, 12 characters):
	log2(76^12) = 12 * log2(76) ~ 75 bits  (an upper bound)
*/
int policy_check(char *password, char *username)
{
	int len = strlen(password);
	int has_lower = 0, has_upper = 0, has_digit = 0, has_special = 0;
	int bad = 0;
	const char *specials = "!@#$%^&*()-_=+";

	for (int i = 0; i < len; i++)
	{
		if (islower((unsigned char)password[i]))
			has_lower = 1;
		else if (isupper((unsigned char)password[i]))
			has_upper = 1;
		else if (isdigit((unsigned char)password[i]))
			has_digit = 1;
		else if (strchr(specials, password[i]) != NULL)
			has_special = 1;
	}
	if (len < 12)
		{ printf("- Password must be at least 12 characters long.\n"); bad = 1; }
	if (!has_lower)
		{ printf("- Password must contain at least one lowercase letter.\n"); bad = 1; }
	if (!has_upper)
		{ printf("- Password must contain at least one uppercase letter.\n"); bad = 1; }
	if (!has_digit)
		{ printf("- Password must contain at least one digit.\n"); bad = 1; }
	if (!has_special)
		{ printf("- Password must contain at least one special character (%s).\n", specials); bad = 1; }
	if (strcmp(password, username) == 0)
		{ printf("- Password cannot be the same as your username.\n"); bad = 1; }
	return (bad);
}

/* returns 1 if the username is already in salt.txt, else 0 */
int check_existing_user(char *username)
{
	char salt[9];

	return (get_user_salt(username, salt));
}

/* 8 random digits. The generator is seeded once in main(). */
void create_salt(char *salt)
{
	for (int i = 0; i < 8; i++)
		salt[i] = '0' + (rand() % 10);
	salt[8] = '\0';
}

/* hash = MD5(password concatenated with salt) */
void create_hash(char *password, char *salt, char *hash)
{
	char combined[PW_LEN + 16];

	snprintf(combined, sizeof(combined), "%s%s", password, salt);
	md5_string(combined, hash);
}
