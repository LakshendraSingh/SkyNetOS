#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_NAME_LENGTH 255
#define MAX_FILES 100

typedef struct {
    char name[MAX_NAME_LENGTH];  
    size_t size;  
    uint8_t* data;
    bool is_directory;
} file_t;

typedef struct {
    char name[MAX_NAME_LENGTH]; 
    file_t* files[MAX_FILES]; 
    size_t file_count;  
} directory_t;

void init_file_system();
void create_file(const char* name);
void write_file(const char* name, const uint8_t* data, size_t size);
int read_file(const char* name, uint8_t* buffer, size_t* size);
void delete_file(const char* name);
void create_directory(const char* name);
void delete_directory(const char* name);
directory_t* get_root_directory(void);

#endif
