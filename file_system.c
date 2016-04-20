#include "file_system.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "kmem_cache.h"

#include <stddef.h>

fs_tree file_system_tree;
lock_descriptor file_system_lock

//Extra functions

void init_file_system_node(struct fs_node** result, const char* name, node_type type) {
    //Try to allocate memory for fs_node structure
    (*result) = kmem_alloc(sizeof(fs_node));
    
    //If it failed, report it 
    if(*result == NULL) {
        printf("Failed to allocate memory for a new file or directory");
        while(1);
    }

    (*result)->name = name;
    (*result)->node_t = type;

    if(type == REGULAR_FILE) {
        //If it is regular file, we need to allocate memory for file content
        (*result)->file_content = kmem_alloc(2 * PAGE_SIZE);
    } else if(type == DIRECTORY) {
        //If it is directory, we need not allocate memory, so, file_content == NULL
        (*result)->file_content = NULL;
    }

    list_init(&((*result)->children));
}

void init_file_system() {
    printf("File system initialization started\n");

    file_system_lock.is_locked = FALSE;

    file_system_tree.root.name = "";
    file_system_tree.root.node_t = DIRECTORY;
    file_system_tree.root.file_content = kmem_alloc(2 * PAGE_SIZE);
    file_system_tree.root.file_content_size = 0;
    list_init(&(file_system_tree.root.children));

    printf("Initialization finished\n");
}

fs_node* find_file_directory(const char* name, node_type type, fs_node* src) {
    if (type == DIRECTORY) {
        printf("Started finding directory named: %s\n", name);
    }

    if (type == REGULAR_FILE) {
        printf("Started finding file named: %s\n", name);
    }

    //First, we checked, maybe our root is the file/directory we are looking for
    if(strcmp(name, "") == 0) {
        printf("It is root\n");
        return &(file_system_tree.root);
    }

    //Checked, that the node src is not null
    if(src == NULL) {
        printf("Src is null\n");
        return NULL;
    }

    /*
     * If the name of src node is the same as name of our file/directory and src node have the same type 
     * (file or directory respectively), we find what we want and return src node
     */ 
    if(strcmp(name, src->name) == 0 && src->node_t == type) {
        printf("We found what we wanted\n");
        return src;
    }

    /*
     * If the type of our src node is REGULAR_FILE or it is a DIRECTORY without children
     * it means that we haven't found what we wanted (now we're in leaf of file_system_tree)
     */ 
    if(src->node_t == REGULAR_FILE || (src->node_t == DIRECTORY && list_empty(&(src->children)))) {
        printf("We haven't found what we wanted\n");
        return NULL;
    }

    /*
     * If we stay here, it means that we are a DIRECTORY with children 
     * we recursively run our find_file_directory function in the subtrees
     * if we found what we wanted in the subtree, we return it's node
     */
    for(list_head* current_node = src->children.next; current_node != &(src->children); current_node = current_node->next) {
        fs_node* subtree_result = find_file_directory(name, type, (fs_node*)current_node);
        if(subtree_result != NULL) {
            printf("We found our file/directory in the subtree\n");
            return subtree_result;
        }
    }

    printf("Finally we haven't found what we wanted\n");
    return NULL;
}

const char* get_directory_name(char* file_name) {
    int length = (int) strlen(file_name);

    //Decrease length until we find name of directory where our file placed (ends with '/')
    while(length >= 0 && file_name[length] != SEPARATOR) {
        --length;
    }

    // Allocate memory for result
    char* result = kmem_alloc(length * sizeof(char));
    //Copy first length symbols of the file_name (real directory name) to the result
    memcpy(result, file_name, length);

    //Make the result directory name ends with '0'
    result[length] = 0;

    return result;
}


//Main functions

/*
 * This method find file or directory in our file_system_tree
 * if it exists, we return its node
 * if it doesn't exist, we creates it and then return its node
 */
struct fs_node* find_create_file_directory(const char* name, node_type type) {
    if (type == DIRECTORY) {
        printf("We run find_create function with directory named: %s\n", name);
    }

    if (type == REGULAR_FILE) {
        printf("We run find_create function with regular file named: %s\n", name);
    }

    // We need locks to prevent changing file_system_tree structure while we're finding smth in it
    lock(&file_system_lock);
    fs_node* result = find_file_directory(name, type, &(file_system_tree.root));
    unlock(&fs_lock);

    //If we didn't find anything, we need to create fs_node for our file/directory
    if(result == NULL) {
        // Initialize new node, using init_file_system_node function
        init_file_system_node(&result, name, type);

        //Find name of directory, where our file placed, using get_directory_name function
        char* directory_name = get_directory_name(name);

        //Return node of our containing directory by name
        fs_node* containing_directory = find_create_file_directory(directory_name, DIRECTORY);
        kmem_free(directory_name);

        lock(&file_system_lock);
        list_add(&(result->node), &(containing_directory->children));
        unlock(&file_system_lock);
    }

    printf("Node of our file/directory is: %p\n", result);
    return result;
}

//And now we can easy create functions open and mkdir with "find_create_file_directory" function

fs_node* open(const char* name) {
    printf("Open a file named: %s\n", name);
    return find_create_file_directory(name, REGULAR_FILE);
}

fs_node* mkdir(const char* name) {
    printf("Mkdir named: %s\n", name);
    return find_create_file_directory(name, DIRECTORY);
}

void close (const char* name) {
    (void) name;
}

void read(fs_node* file, uint64_t size, uint64_t offset, void* buffer) {
    if(file == NULL || buffer == NULL) {
        printf("File and buffer can't be NULL");
        return;
    }

    //We need locks here because we don't want others change smth while we're reading file
    lock(&file_system_lock);
    
    //Length shows us real length of what we want to read
    uint64_t length = (file->file_content_size - offset) < size ? file->file_content_size : size;
    
    //Copy length symbols of our file (started where file_content starts + offset) to the buffer
    memcpy(buffer, (char*)(file->file_content) + offset, length);
    unlock(&file_system_lock);
}

void write(fs_node* file, uint64_t size, uint64_t offset, void* buffer) {
    if(file == NULL || buffer == NULL) {
        printf("File and buffer can't be NULL");
        return;
    }

    //We need locks here because we don't want others change smth while we're writing to file
    lock(&file_system_lock);
    //Copy size symbols from the buffer to the file (start writing where file_content starts + offset)
    memcpy((char*)(file->file_content) + offset, buffer, size);

    //If size of our file's content become greater, than we increase file_content_size
    if(file->file_content_size < offset + size) {
        file->file_content_size = offset + size;
    }
    unlock(&file_system_lock);
}

int readdir(fs_node* dir, uint64_t size, uint64_t offset, fs_node* buffer) {
    if(dir == NULL || buffer == NULL) {
        printf("Directory and buffer can't be NULL");
        return 0;
    }

    //We need locks here because we don't want others change smth while we're reading the directory
    lock(&file_system_lock);
    uint64_t i = 0;
    int result = 0;

    printf("We are going to print directory %s:\n", dir->name);
    for(struct list_head* current = dir->children.next; current != &(dir->children); current = current->next) {
        if(((fs_node*)current)->node_t == DIRECTORY) {
            printf("DIRECTORY %s\n", ((fs_node*)current)->name);
        } else {
            printf("REGULAR_FILE %s\n", ((fs_node*)current)->name);
        }

        if(i >= offset && i < offset + size) {
            buffer[i - offset] = *((fs_node*)current);
            ++result;
        }
        ++i;
    }
    unlock(&file_system_lock);

    return result;
}


