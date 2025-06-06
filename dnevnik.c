#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "dnevnik.h"
#include "vec.h"

void main_menu(){
    printf("sssssssss");
}
void create_story() {
    char title[MAX_DATE_LEN];
    char date[MAX_DATE_LEN];
    char story[MAX_STORY_LEN];
    
    printf("Въведете дата (YYYY-MM-DD): ");
    scanf("%10s", date);
    getchar(); // премахване на нов ред

    printf("Въведете заглавие (до 30 символа): ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0;  // премахване на newline

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

void list_stories(int per_page) {
    int cur_page = 0;
    vec_str_t v;
    vec_init(&v);
    struct dirent *de;

    DIR *dr = opendir(STORIES_DIR);

    if (dr == NULL)
    {
        printf("Could not open current directory" );
        return 0;
    }

    while ((de = readdir(dr)) != NULL)
            printf("%s\n", de->d_name);
            vec_push(&v, de->d_name);

    closedir(dr);

    vec_str_t dati;
    vec_init(&dati);
    vec_str_t zaglaviq;
    vec_init(&zaglaviq);
    vec_str_t temp;
    vec_init(&temp);

    for(int i = 0; i<vec_size; i++){
        // Extract the first token
        char * token = strtok(v[i], "_");
        // loop through the string to extract all other tokens
        while( token != NULL ) {
            printf( " %s\n", token ); //printing each token
            vec_push(&temp, token);
            token = strtok(NULL, "_");
        }
        vec_push(&dati, temp[0]);
        vec_push(&zaglaviq, temp[1]);
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
        main_menu();
    } else if (isdigit(input)) {
        int number = input - '0'; // Convert char to actual integer 0-9
        read_story(v[number+(cur_page*10)]);
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

    fclose(f);
}
