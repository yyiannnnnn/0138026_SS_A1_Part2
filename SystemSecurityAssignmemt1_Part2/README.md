# Authentication and access control system

Written to understand how shadow-style password storage works (salt + hash), how a login is validated, and how the Bell–LaPadula model controls file access.

# Project layout
File & Contents
main.c: start-up, MD5 self test, `-i` or login mode 
account_creation.c: `account_creation`, `policy_check`, `check_existing_user`, `create_salt`, `create_hash` 
logging_in.c: `logging_in`, `get_user_salt`, `get_user_shadow` 
file_menu.c: in-memory file records, Bell–LaPadula checks, menu loop, Files.store save/load 
util.c: input helpers (`read_line`, `read_password`, `clearing_buffer_STDIN`) 
md5.c: own MD5 implementation (no OpenSSL needed) 
header.h: shared includes, struct, prototypes 

# Compile
bash
gcc *.c -o FileSystem

# Run
bash
touch salt.txt shadow.txt     # blank files (also created automatically if missing)
./FileSystem -i               # create a user
./FileSystem                  # log in

# Creating a user
1. Prompt for a username; stop if it is already in salt.txt.
2. Prompt for password + confirmation (3 attempts). `policy_check()` lists every unmet requirement.
3. Prompt for clearance 0–3.
4. `create_salt()` makes 8 random digits (generator seeded once in `main`).
5. `create_hash()` = MD5(password + salt), e.g. `alphabet12345678`.
6. Append `user:salt` to salt.txt and `user:hash:clearance` to shadow.txt.

# Logging in
1. Prompt for username and password.
2. `get_user_salt()` finds the salt in salt.txt and the entered password is hashed with it.
3. `get_user_shadow()` fetches the stored hash and clearance.
4. `strcmp(computed, stored)`; mismatch -> authentication fails and the program exits.
5. On success the clearance is reported and `file_menu_loop()` starts (Files.store is loaded first if present).

At start-up `MD5("This is a test")` is computed and printed (not hard coded): `ce114e4501d2f4e2dcea3e17b546f339`.

# Password policy
- At least 12 characters; at least one lower case, upper case, digit, and special character from `!@#$%^&*()-_=+`
- Must not equal the username; must match the confirmation

**Entropy:** character set = 26 + 26 + 10 + 14 = 76, so N = 76^12 ≈ 3.71 × 10^22 and
entropy = log2(N) = 12 × log2(76) ≈ **75 bits** (upper bound, assumes uniformly random characters).

**Time to crack:** expected time = N / (2R). With R = 1000 guesses/s: 3.71×10^22 / 2000 ≈ 1.86 × 10^19 s ≈ 5.9 × 10^11 years.
(Real offline attacks on MD5 are far faster; MD5 is used only because the assignment requires it.)

# Bell–LaPadula
Levels 0 < 1 < 2 < 3. A new file gets its owner's clearance.
Read: user ≥ file, no read up 
Append: user ≤ file, no write down 
Write: user == file, needs both read and write 

Example: john (1) cannot READ or WRITE alpha (3) but may APPEND to it; jane (3) can READ beta (1) but cannot WRITE or APPEND to it.
