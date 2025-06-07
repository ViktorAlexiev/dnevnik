#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "dnevnik.h"
#include "vec.h"

//ако неща не бачка въведи няква променлива за state на програмата



void release_mem(char** v, char** dati, char** zaglaviq){
    vector_free(&v);
    vector_free(&dati);
    vector_free(&zaglaviq);
}
void main_menu(char** v, char** dati, char** zaglaviq){
    printf("1.създай история");
    printf("2.списък с истории");
    printf("Натиснете q за затваряне на програмата");
    char input;
    input = getchar();
    if (input == '1') {
        create_story();
    } else if (input == '2') {
        list_stories(10, v, dati, zaglaviq);
    } else if (input == 'q') {
        exit(1);
    } else {
        printf("Грешна команда. Въведи я отново");
    }
}

void search(char** v, char** dati, char** zaglaviq){
    char date[MAX_DATE_LEN];
    printf("Въведете дата (YYYY-MM-DD): ");
    scanf("%10s", date);
    getchar(); // премахване на нов ред
    char input;
    input = getchar();
    if(date == 'q'){
        list_stories(10, v, dati, zaglaviq);
    } else if (input == 'q') {
        main_menu(v, dati, zaglaviq);
    }
    //търсене по дата
    //показване
    //изчакване на команда или за назад или за четене
}
void create_story() {
    char title[MAX_ENTRIES][MAX_DATE_LEN];
    char date[MAX_ENTRIES][MAX_DATE_LEN];
    char story[MAX_STORY_LEN];
    
    printf("Въведете дата (YYYY-MM-DD): ");
    scanf("%10s", date);
    getchar(); // премахване на нов ред

    printf("Въведете заглавие (до 30 символа): ");
    fgets(title, sizeof(title), stdin);
    //title[strcspn(title, "\n")] = 0;  // премахване на newline

    printf("Въведете историята (край с EOF - Ctrl+D):\n");
    fgets(story, sizeof(story), stdin);

    char filename[MAX_FILENAME_LEN];
    snprintf(filename, sizeof(filename), "%s_%s.txt", date, title);

    char path[MAX_FILENAME_LEN];
    snprintf(path, sizeof(path), "%s/%s", STORIES_DIR, filename);

    FILE* f = fopen(path, "w");
    if (!f) {
        perror("Грешка при записване");
        return;
    }

    fprintf(f, "%s\n%s\n%s\n", date, title, story);
    fclose(f);
}

void insertion_sort(char** v, char** dati, char** zaglaviq, int n) {
    for (int i = 1; i < n; i++) {
        char date_key[MAX_DATE_LEN];
        char title_key[MAX_TITLE_LEN];

        strcpy(date_key, dati[i]);
        strcpy(title_key, zaglaviq[i]);

        int j = i - 1;
        while (j >= 0 && strcmp(dati[j], date_key) > 0) {
            strcpy(dati[j + 1], dati[j]);
            strcpy(zaglaviq[j + 1], zaglaviq[j]);
            j--;
        }
        strcpy(dati[j + 1], date_key);
        strcpy(zaglaviq[j + 1], title_key);
    }
}

void list_stories(int per_page, char** v, char** dati, char** zaglaviq) {
    
    int cur_page = 0;
    
    struct dirent *de;

    DIR *dr = opendir(STORIES_DIR);

    if (dr == NULL)
    {
        printf("Could not open current directory" );
        return 0;
    }

    while ((de = readdir(dr)) != NULL)
            printf("%s\n", de->d_name);
            vector_add(&v, de->d_name);

    closedir(dr);

    char* temp = vector_create();

    int vec_size = 0;

    for(int i = 0; i<vec_size; i++){
        // Extract the first token
        char * token = strtok(v[i], "_");
        vector_free(&temp);
        char* temp = vector_create();
        while( token != NULL ) {
            printf( " %s\n", token ); //printing each token
            vector_add(&temp, token);
            token = strtok(NULL, "_");
        }
        vector_add(&dati, temp[0]);
        vector_add(&zaglaviq, temp[1]);
    }

    insertion_sort(v, dati, zaglaviq, vec_size);
    for(int i = 0; i < vec_size; i++){
        char result[45];
        strcat(result, dati[i]);
        strcat(result, '_');
        strcat(result, zaglaviq[i]);
        strcpy(v[i], result);
    }

    for(int i = cur_page*10; i<per_page; i++){
        printf("%d.%s", i/(cur_page*10), v[i]);
        if(i == vec_size){
            break;
        }
    }

    char input;
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
        main_menu(v, dati, zaglaviq);
    } else if (isdigit(input)) {
        int number = input - '0'; // Convert char to actual integer 0-9
        read_story(v[number+(cur_page*10)]);
    } else if (input == 's') {
        main_menu(v, dati, zaglaviq);
    } else {
        printf("Грешна команда. Въведи я отново");
    }

}

void read_story(const char* filename) {
    char path[MAX_FILENAME_LEN];
    snprintf(path, sizeof(path), "%s/%s", STORIES_DIR, filename);

    FILE* f = fopen(path, "r");
    if (!f) {
        perror("Грешка при записване");
        return;
    }

    char story_buffer[MAX_STORY_LEN];

    while(fgets(story_buffer, MAX_STORY_LEN, f)) {
        printf("%s", story_buffer);
    }

    fclose(f);
}

int main(){
    char** v = vector_create();
    char** dati = vector_create();
    char** zaglaviq = vector_create();
    main_menu(v, dati, zaglaviq);

    release_mem(&v, &dati, &zaglaviq);
    return 0;
}