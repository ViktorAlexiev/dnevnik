#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "dnevnik.h"
#include "vec.h"
#include <sys/stat.h>//for stat()
#include <openssl/evp.h>//for encription/decription
#include <openssl/rand.h>//forencription/decription
#include <ctype.h>//for isdigit()

int match_count = 0;
char CURRENT_USER[50];
char USER_PASSWORD[50];
char STORIES_DIR[100];

void release_mem(char** v, char** dates, char** titles){
    vector_free(&v);
    vector_free(&dates);
    vector_free(&titles);
}

int is_number(const char* s) {
    while (*s) {
        if (!isdigit(*s)) return 0;
        s++;
    }
    return 1;
}


void encrypt(const char *plaintext, unsigned char **ciphertext, int *len, const char *password) {
    EVP_CIPHER_CTX *ctx;
    int len1, len2;

    unsigned char key[32];  // AES-256 = 256 bits = 32 bytes
    unsigned char iv[16];   // AES block size = 16 bytes

    EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), NULL,(const unsigned char *)password, strlen(password), 1, key, iv);

    *ciphertext = malloc(strlen(plaintext) + EVP_MAX_BLOCK_LENGTH + 16);
    if (!*ciphertext) {
        perror("Memory allocation failed");
        exit(1);
    }

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, *ciphertext, &len1, (const unsigned char *)plaintext, strlen(plaintext));
    EVP_EncryptFinal_ex(ctx, *ciphertext + len1, &len2);
    *len = len1 + len2;

    EVP_CIPHER_CTX_free(ctx);
}

void decrypt(unsigned char *ciphertext, int ciphertext_len, char *plaintext_out, const char *password) {
    EVP_CIPHER_CTX *ctx;
    int len1, len2;

    unsigned char key[32];
    unsigned char iv[16];

    EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), NULL,(const unsigned char *)password, strlen(password), 1, key, iv);

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, (unsigned char *)plaintext_out, &len1, ciphertext, ciphertext_len);
    EVP_DecryptFinal_ex(ctx, (unsigned char *)plaintext_out + len1, &len2);
    plaintext_out[len1 + len2] = '\0';

    EVP_CIPHER_CTX_free(ctx);
}


void main_menu(char** v, char** dates, char** titles){
    printf("1.Make a story\n");
    printf("2.Story list\n");
    printf("Press 'q' to exit\n");
    char input;
    do {
        input = getchar();
        if (input == '1') {
            create_story(v, dates, titles);
        } else if (input == '2') {
            list_stories(10, v, dates, titles);
        } else if (input == 'q') {
            exit(1);
        } else {
            printf("Wrong command. Enter Again\n");
        }
    }while(input!='1' || input!='2' || input!='q');
}

void login() {
    printf("Enter username: ");
    scanf("%49s", CURRENT_USER);
    getchar();

    printf("Enter password: ");
    scanf("%49s", USER_PASSWORD);
    getchar();

    snprintf(STORIES_DIR, sizeof(STORIES_DIR), "stories/%s", CURRENT_USER);

    struct stat st = {0};
    if (stat(STORIES_DIR, &st) == -1) {
        mkdir(STORIES_DIR);//if on Linux add a second parameter being 0700
    }
}

int* find_date_indexes(char** arr, int n, const char* target) {
    int left = 0, right = n - 1, first = -1;

    // Binary search to find the first occurrence of target
    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = strcmp(arr[mid], target);
        if (cmp == 0) {
            first = mid;
            right = mid - 1;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    match_count = 0;

    if (first == -1) {
        return NULL;
    }

    // Count how many elements match
    int i = first;
    while (i < n && strcmp(arr[i], target) == 0) {
        match_count++;
        i++;
    }

    int* indexes = malloc(match_count * sizeof(int));
    if (!indexes) {
        perror("Memory allocation failed");
        exit(1);
    }

    for (int j = 0; j < match_count; j++) {
        indexes[j] = first + j;
    }

    return indexes;
}

void search(char** v, char** dates, char** titles, int n){
    char target_date[MAX_DATE_LEN];
    printf("Enter date (YYYY-MM-DD): ");
    scanf("%10s", target_date);
    getchar();

    //menu navigation/cancel search
    if(strcmp(target_date, "q") == 0){
        list_stories(10, v, dates, titles);
    } else if (strcmp(target_date, "m") == 0) {
        main_menu(v, dates, titles);
    }

    int* indexes = find_date_indexes(dates, n, target_date);
    
    for(int i = 0; i<match_count; i++){
        printf("%d.%s", i, v[indexes[i]]);
    }

    char input[100];
    do{
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "q") == 0) {
            list_stories(10, v, dates, titles);
        }else if (strcmp(input, "m") == 0) {
            main_menu(v, dates, titles);
        } else if (is_number(input)) {
            int number = atoi(input);
            read_story(v[indexes[number]], v, dates, titles);
        } else {
            printf("Invalid entry. Try again.\n");
        }  
    }while(strcmp(input, "q") == 1 || strcmp(input, "m") == 1);
}

void create_story(char** v, char** dates, char** titles) {
    char title[MAX_DATE_LEN];
    char date[MAX_DATE_LEN];
    char story[MAX_STORY_LEN];
    
    printf("Enter date (YYYY-MM-DD): ");
    scanf("%10s", date);
    getchar();

    printf("Enter title (up to 30 symbols): ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0;

    printf("Enter the story (end with EOF - Ctrl+D):\n");
    fgets(story, sizeof(story), stdin);

    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s_%s.txt", date, title);

    char path[MAX_FILENAME_LEN];
    snprintf(path, sizeof(path), "%s/%s", STORIES_DIR, filename);


    unsigned char* ciphertext = NULL;
    int ciphertext_len;
    encrypt(story, &ciphertext, &ciphertext_len, USER_PASSWORD);

    FILE* f = fopen(path, "wb");
    if (!f) {
        perror("Error during writing");
        return;
    }

    fwrite(ciphertext, 1, ciphertext_len, f);
    fclose(f);
    free(ciphertext);
    printf("Story saved\n");
    main_menu(v, dates, titles);
}

void insertion_sort(char** v, char** dates, char** titles, int n) {
    for (int i = 1; i < n; i++) {
        char date_key[MAX_DATE_LEN];
        char title_key[MAX_TITLE_LEN];

        strcpy(date_key, dates[i]);
        strcpy(title_key, titles[i]);

        int j = i - 1;
        while (j >= 0 && strcmp(dates[j], date_key) > 0) {
            strcpy(dates[j + 1], dates[j]);
            strcpy(titles[j + 1], titles[j]);
            j--;
        }
        strcpy(dates[j + 1], date_key);
        strcpy(titles[j + 1], title_key);
    }
}

void list_stories(int per_page, char** v, char** dates, char** titles) {
    
    int cur_page = 0;
    struct dirent* de;
    DIR* dr = opendir(STORIES_DIR);

    if (dr == NULL)
    {
        printf("Could not open current directory\n");
        return;
    }

    int j = 0;
    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){//skips . and ..
            continue;
        }
        printf("%d. %s\n", j, de->d_name);
        vector_add(&v, de->d_name);
        j++;
    }
    
    closedir(dr);

    char* temp = vector_create();

    int vec_size = vector_size(v);
    for (int i = 0; i < vec_size; i++) {
        
        char* filename_copy = strdup(v[i]);
        if (!filename_copy) {
            perror("strdup failed");
            exit(1);
        }
        
        char* token = strtok(filename_copy, "_");
        if (!token){
            continue;
        }
        vector_add(&dates, strdup(token));
    
        token = strtok(NULL, "_");
        if (!token){
            continue;
        }

        vector_add(&titles, strdup(token));
        free(filename_copy);
    }
    insertion_sort(v, dates, titles, vec_size);
    for (int i = 0; i < vec_size; i++) {
        char result[MAX_FILENAME_LEN] = "";
        strcat(result, dates[i]);
        strcat(result, "_");
        strcat(result, titles[i]);
        strcpy(v[i], result);
    }

    int start = cur_page * per_page;
    int end = start + per_page;
    if (end > vec_size){
        end = vec_size;
    } 

    for (int i = start; i < end; i++) {
        printf("%d. %s\n", i, v[i]);
    }

    char input;
    do{
        input = getchar();
        if (input == 'z') {
            if(cur_page>=0){
                cur_page-=1;
                for(int i = cur_page*10; i<per_page; i++){
                    printf("%d.%s", i/(cur_page*10), v[i]);
                    if(i == vec_size){
                        break;
                    }
                }
            }
        } else if (input == 'x') {
            cur_page+=1;
            for(int i = cur_page*10; i<per_page; i++){
                printf("%d.%s", i/(cur_page*10), v[i]);
                if(i == vec_size){
                    break;
                }
            }
        } else if (input == 'q') {
            main_menu(v, dates, titles);
        } else if (isdigit(input)) {
            int number = input - '0';
            read_story(v[number+(cur_page*10)], v, dates, titles);
        } else if (input == 's') {
            search(v, dates, titles, vec_size);
        } else {
            printf("Wrong command. Enter Again\n");
        }
    }while(input!='z' || input!='x' || input!= 'q' || input!='s');

}

void read_story(const char* filename, char** v, char** dates, char** titles) {
    char path[MAX_FILENAME_LEN];
    snprintf(path, sizeof(path), "%s/%s", STORIES_DIR, filename);

    FILE* f = fopen(path, "r");
    if (!f) {
        perror("Error during saving");
        return;
    }

    unsigned char buffer[MAX_STORY_LEN];

    int bytes_read = fread(buffer, 1, MAX_STORY_LEN, f);
    fclose(f);

    char decrypted[MAX_STORY_LEN];
    decrypt(buffer, bytes_read, decrypted, USER_PASSWORD);

    printf("%s", decrypted);

    char input;
    do{
        input = getchar();
        if (input == 'q') {
            list_stories(10, v, dates, titles);
        } else if (input == 'm') {
            main_menu(v, dates, titles);
        } else {
            printf("Wrong command. Enter Again\n");
        }
    }while(input!= 'q' || input!='m');
}

int main(){
    char** v = vector_create();
    char** dates = vector_create();
    char** titles = vector_create();

    login();
    main_menu(v, dates, titles);

    release_mem(v, dates, titles);
    return 0;
}