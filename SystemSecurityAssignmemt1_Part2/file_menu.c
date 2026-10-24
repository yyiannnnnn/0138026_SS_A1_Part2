#include "header.h"

FileRecord file_list[MAX_FILES];
int file_count = 0;

/* index in file_list, or -1 if not found */
int find_file(char *filename)
{
	for (int i = 0; i < file_count; i++)
		if (strcmp(file_list[i].filename, filename) == 0)
			return (i);
	return (-1);
}

void add_file(char *filename, char *owner, int classification)
{
	if (file_count >= MAX_FILES)
	{
		printf("File list is full. Cannot create more files.\n");
		return;
	}
	snprintf(file_list[file_count].filename, sizeof(file_list[file_count].filename), "%s", filename);
	snprintf(file_list[file_count].owner, sizeof(file_list[file_count].owner), "%s", owner);
	file_list[file_count].classification = classification;
	file_count++;
}

int get_classification(int index)
{
	return (file_list[index].classification);
}

void list_files(void)
{
	if (file_count == 0)
	{
		printf("No files currently exist.\n");
		return;
	}
	printf("Filename\tOwner\tClassification\n");
	for (int i = 0; i < file_count; i++)
		printf("%s\t%s\t%d\n", file_list[i].filename, file_list[i].owner, file_list[i].classification);
}

/* human readable: filename:owner:classification, one per line */
void save_files_store(void)
{
	FILE *fp = fopen("Files.store", "w");

	if (fp == NULL)
	{
		printf("Error: could not open Files.store for saving.\n");
		return;
	}
	for (int i = 0; i < file_count; i++)
		fprintf(fp, "%s:%s:%d\n", file_list[i].filename, file_list[i].owner, file_list[i].classification);
	fclose(fp);
	printf("Files saved to Files.store.\n");
}

/* loaded once after login, if Files.store exists */
void load_files_store(void)
{
	FILE *fp = fopen("Files.store", "r");
	char line[256], fname[100], owner[USER_LEN];
	int classification;

	if (fp == NULL)
		return;
	while (fgets(line, sizeof(line), fp) != NULL && file_count < MAX_FILES)
	{
		if (sscanf(line, "%99[^:]:%20[^:]:%d", fname, owner, &classification) != 3)
			continue;
		add_file(fname, owner, classification);
	}
	fclose(fp);
}

/*
Bell-LaPadula. Levels 0 < 1 < 2 < 3, so "dominates" is simply >=.
	read   : user >= file    (no read up)
	append : user <= file    (no write down; writing up is allowed)
	write  : user == file    (write needs read and write access)
*/
int can_read(int user, int file)
{
	return (user >= file);
}

int can_append(int user, int file)
{
	return (user <= file);
}

int can_write(int user, int file)
{
	return (can_read(user, file) && can_append(user, file));
}

/* (C)reate (A)ppend (R)ead (W)rite (L)ist (S)ave (E)xit */
void file_menu_loop(int clearance, char *username)
{
	char options[16];
	char filename[100];
	char confirm[16];
	int  idx;
	int  flag = 0;

	load_files_store();
	while (flag == 0)
	{
		printf("============================================\n");
		printf("(C)reate\n(A)ppend\n(R)ead\n(W)rite\n(L)ist\n(S)ave\n(E)xit\n");
		read_line("Enter the character of the action you want to do: ", options, sizeof(options));
		if (strlen(options) != 1 || strchr("CARWLSEcarwlse", options[0]) == NULL)
		{
			printf("Invalid input, try again\n");
			continue;
		}
		options[0] = toupper((unsigned char)options[0]);

		if (strchr("CARW", options[0]) != NULL)
		{
			read_line("Filename: ", filename, sizeof(filename));
			if (filename[0] == '\0' || strchr(filename, ':') != NULL)
			{
				printf("Invalid filename (empty or contains ':').\n");
				continue;
			}
		}
		idx = (strchr("CARW", options[0]) != NULL) ? find_file(filename) : -1;

		switch (options[0])
		{
			case 'C':
				if (idx != -1)
					printf("File '%s' already exists.\n", filename);
				else
				{
					add_file(filename, username, clearance);
					printf("File '%s' created with classification %d.\n", filename, clearance);
				}
				break;
			case 'R':
				if (idx == -1)
					printf("File '%s' does not exist.\n", filename);
				else if (can_read(clearance, get_classification(idx)))
					printf("Read access granted for '%s'.\n", filename);
				else
					printf("Read access denied for '%s': clearance too low (no read-up).\n", filename);
				break;
			case 'A':
				if (idx == -1)
					printf("File '%s' does not exist.\n", filename);
				else if (can_append(clearance, get_classification(idx)))
					printf("Append access granted for '%s'.\n", filename);
				else
					printf("Append access denied for '%s': clearance too high (no write-down).\n", filename);
				break;
			case 'W':
				if (idx == -1)
					printf("File '%s' does not exist.\n", filename);
				else if (can_write(clearance, get_classification(idx)))
					printf("Write access granted for '%s'.\n", filename);
				else
					printf("Write access denied for '%s': write needs clearance equal to classification.\n", filename);
				break;
			case 'L':
				list_files();
				break;
			case 'S':
				save_files_store();
				break;
			case 'E':
				read_line("Shut down the FileSystem? (Y)es or (N)o: ", confirm, sizeof(confirm));
				if (confirm[0] == 'Y' || confirm[0] == 'y')
				{
					printf("Shutting down\n");
					flag = 1;
				}
				break;
		}
	}
}
