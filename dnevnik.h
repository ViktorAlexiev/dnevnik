#ifndef DNEVNIK_H
#define DNEVNIK_H

#define MAX_TITLE_LEN 30
#define MAX_DATE_LEN 11  // формат: YYYY-MM-DD
#define MAX_STORY_LEN 5000
#define MAX_FILENAME_LEN 45
//#define STORIES_DIR "stories"
#define MAX_USER_LEN 50

void release_mem(char** v, char** dati, char** zaglaviq);
void main_menu(char** v, char** dati, char** zaglaviq);      
int* find_date_indexes(char** arr, int n, const char* target);
void insertion_sort(char** v, char** dati, char** zaglaviq, int n);
void search(char** v, char** dati, char** zaglaviq, int n);      
void create_story(char** v, char** dati, char** zaglaviq);          
void list_stories(int per_page, char** v, char** dati, char** zaglaviq); 
void read_story(const char* filename, char** v, char** dati, char** zaglaviq);  
void login();
void encrypt_story(const char* plaintext, const char* password, unsigned char** ciphertext, int* len);
void decrypt_story(unsigned char* ciphertext, int ciphertext_len, const char* password, char* plaintext_out);

#endif
