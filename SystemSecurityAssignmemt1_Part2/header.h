#ifndef HEADER_H
# define HEADER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <time.h>
# include <stdint.h>
# include <unistd.h>
# include <termios.h>

# define MAX_FILES 256
# define USER_LEN  21   /* 20 chars + '\0' */
# define PW_LEN    65   /* 64 chars + '\0' */

typedef struct s_file_record
{
	char filename[100];
	char owner[USER_LEN];
	int  classification;
}	FileRecord;

/* main.c */
int  main(int argc, char **argv);

/* md5.c (own MD5 implementation, no OpenSSL needed) */
void md5_string(const char *input, char output_hex[33]);

/* util.c */
void clearing_buffer_STDIN(void);
void read_line(const char *prompt, char *buf, size_t size);
void read_password(const char *prompt, char *buf, size_t size);

/* account_creation.c */
void account_creation(void);
int  policy_check(char *password, char *username);
int  check_existing_user(char *username);
void create_salt(char *salt);
void create_hash(char *password, char *salt, char *hash);

/* logging_in.c */
void logging_in(void);
int  get_user_salt(char *username, char *salt_out);
int  get_user_shadow(char *username, char *hash_out, int *clearance_out);

/* file_menu.c */
int  find_file(char *filename);
void add_file(char *filename, char *owner, int classification);
int  get_classification(int index);
void list_files(void);
void save_files_store(void);
void load_files_store(void);
int  can_read(int user, int file);
int  can_append(int user, int file);
int  can_write(int user, int file);
void file_menu_loop(int clearance, char *username);

#endif
