/*
 * file_system.c
 * This file contains implementations of the file system operations
 * for creating, reading, writing, and deleting files and directories.
 */

#include "file_system.h" 
#include "string.h"
#include "error_codes.h"
#include "kernel.h"
#include "memory_management.h"

static directory_t root_directory;

directory_t* get_root_directory(void) {
    return &root_directory;
}


void init_file_system() {
    strcpy(root_directory.name, "root"); 
    root_directory.file_count = 0;
}

/**
 * Finds the index of a file in the root directory.
 * @return Index or -1 if not found.
 */
static int find_file_index(const char* name) {
    for (size_t i = 0; i < root_directory.file_count; i++) {
        if (strcmp(root_directory.files[i]->name, name) == 0)
            return (int)i;
    }
    return -1;
}

/**
 * Creates a new file.
 */
void create_file(const char* name) {
    if (find_file_index(name) != -1) {
        err_handler(ERR_FILE_EXISTS);
        return;
    }

    file_t* new_file = (file_t*)malloc(sizeof(file_t));
    if (new_file) {
        strcpy(new_file->name, name);
        new_file->size = 0; 
        new_file->data = NULL;
        new_file->is_directory = false;
        root_directory.files[root_directory.file_count++] = new_file;
    } else {
        err_handler(ERR_OUT_OF_MEMORY);
    }
}

/**
 * Creates a new directory.
 */
void create_directory(const char* name) {
    if (find_file_index(name) != -1) {
        err_handler(ERR_FILE_EXISTS);
        return;
    }

    file_t* new_dir = (file_t*)malloc(sizeof(file_t));
    if (new_dir) {
        strcpy(new_dir->name, name);
        new_dir->is_directory = true;
        
        directory_t* subdir = (directory_t*)malloc(sizeof(directory_t));
        if (subdir) {
            strcpy(subdir->name, name);
            subdir->file_count = 0;
            new_dir->data = (uint8_t*)subdir;
            new_dir->size = sizeof(directory_t);
            root_directory.files[root_directory.file_count++] = new_dir;
        } else {
            free(new_dir);
            err_handler(ERR_OUT_OF_MEMORY);
        }
    } else {
        err_handler(ERR_OUT_OF_MEMORY);
    }
}

/**
 * Writes data to a file.
 */
void write_file(const char* name, const uint8_t* data, size_t size) {
    int idx = find_file_index(name);
    if (idx != -1) {
        // Free old data to prevent memory leak
        if (root_directory.files[idx]->data) {
            free(root_directory.files[idx]->data);
        }
        root_directory.files[idx]->data = (uint8_t*)malloc(size);
        if (root_directory.files[idx]->data) {
            memcpy(root_directory.files[idx]->data, data, size);
            root_directory.files[idx]->size = size; 
        } else {
            err_handler(ERR_OUT_OF_MEMORY);
        }
    } else {
        err_handler(ERR_FILE_NOT_FOUND);
    }
}

/**
 * Reads data from a file.
 */
int read_file(const char* name, uint8_t* buffer, size_t* size) {
    int idx = find_file_index(name);
    if (idx != -1) {
        memcpy(buffer, root_directory.files[idx]->data, root_directory.files[idx]->size);
        *size = root_directory.files[idx]->size; 
        return 0;
    }
    *size = 0;
    return ERR_FILE_NOT_FOUND;
}

/**
 * Deletes a file.
 */
void delete_file(const char* name) {
    int idx = find_file_index(name);
    if (idx != -1) {
        free(root_directory.files[idx]->data);  
        free(root_directory.files[idx]);  
        for (size_t j = (size_t)idx; j < root_directory.file_count - 1; j++) {
            root_directory.files[j] = root_directory.files[j + 1];
        }
        root_directory.file_count--;
    } else {
        err_handler(ERR_FILE_NOT_FOUND);
    }
}

/**
 * Deletes a directory and its contents recursively.
 */
void delete_directory(const char* name) {
    int idx = find_file_index(name);
    if (idx != -1) {
        file_t* dir_file = root_directory.files[idx];
        if (!dir_file->is_directory) {
            err_handler(ERR_FILE_NOT_FOUND);
            return;
        }

        directory_t* subdir = (directory_t*)dir_file->data;
        if (subdir) {
            for (size_t i = 0; i < subdir->file_count; i++) {
                file_t* item = subdir->files[i];
                if (item->is_directory) {
                    free(item->data); // Free nested directory_t
                } else {
                    free(item->data); // Free file data
                }
                free(item);
            }
            free(subdir);
        }
        free(dir_file);

        for (size_t j = (size_t)idx; j < root_directory.file_count - 1; j++) {
            root_directory.files[j] = root_directory.files[j + 1];
        }
        root_directory.file_count--;
    } else {
        err_handler(ERR_FILE_NOT_FOUND);
    }
}