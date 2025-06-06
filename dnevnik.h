#ifndef DNEVNIK_H
#define DNEVNIK_H

#define MAX_TITLE_LEN 30
#define MAX_DATE_LEN 11  // формат: YYYY-MM-DD
#define MAX_STORY_LEN 5000
#define MAX_FILENAME_LEN 45
#define STORIES_DIR "stories"

// Структура за дневник (ако ти трябва)
typedef struct {
    char date[MAX_DATE_LEN];
    char title[MAX_TITLE_LEN + 1];
} DiaryEntry;

// Декларации на функциите
void main_menu();      
void search();      
void create_story();             // Добавяне на нова история
void list_stories(int per_page); // Показване на списък с истории
void read_story(const char*);    // Показване на история от файл

#endif
