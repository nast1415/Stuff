#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "locks.h"
#include "list.h"

#define SEPARATOR '/'

typedef enum {DIRECTORY, REGULAR_FILE} node_type;

typedef struct list {
    struct list_head* root;
} list;

typedef struct fs_node {
    struct list_head node;
    const char* name;
    node_type node_t; // Type can be DIRECTORY or REGULAR_FILE
    void* file_content; //If node_t == DIRECTORY, then file_content == NULL
    uint64_t file_content_size;
    struct list_head children; //If node_t == REGULAR_FILE, then this node haven't got children and root == NULL
} fs_node;

typedef struct fs_tree {
    struct fs_node root;
} fs_tree;

extern fs_tree file_system_tree;
extern lock_descriptor file_system_lock;

//Extra functions


void init_file_system_node(fs_node** result, const char* name, node_type type); //This method initialized node of our file system (allocate memory for node and, if it a regular file, allocate memory for content)
void init_file_system(); //This method initialized our file system

fs_node* find_file_directory(const char* name, node_type type, fs_node* src); //Try to found file/directory named "name" in directory src, returns its node if it exists and NULL otherwise

char* get_directory_name(const char* file_name); //Return the name of the directory, where the file named "file_name" placed


//Main functions
fs_node* open(char* name);
fs_node* mkdir(char* name);

void close(char* name);
void read(fs_node* file, uint64_t size, uint64_t offset, void* buffer);
void write(fs_node* file, uint64_t size, uint64_t offset, void* buffer);

int readdir(fs_node* dir, uint64_t size, uint64_t offset, fs_node* buffer);


#endif /* __FILE_SYSTEM_H__ */
