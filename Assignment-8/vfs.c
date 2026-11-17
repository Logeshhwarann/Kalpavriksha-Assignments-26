#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50
#define MAX_LINE 256

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

typedef struct FreeBlock {
    int block_index;
    struct FreeBlock* prev;
    struct FreeBlock* next;
} FreeBlock;

typedef enum {
    NODE_DIR,
    NODE_FILE
} NodeType;

typedef struct FileNode {
    char name[MAX_NAME];
    NodeType type;
    struct FileNode* parent;
    
    struct FileNode* child_head;
    
    int* block_pointers;
    int num_blocks;
    int content_size;
    
    struct FileNode* next;
    struct FileNode* prev;
} FileNode;

FreeBlock* freeListHead = NULL;
FreeBlock* freeListTail = NULL;
FileNode* root = NULL;
FileNode* cwd = NULL;
int used_blocks_count = 0;

void init_vfs();
void free_block_push_back(int index);
int free_block_pop_front();
FileNode* create_node(const char* name, NodeType type, FileNode* parent);
void add_child(FileNode* parent, FileNode* child);
void remove_child(FileNode* parent, FileNode* child);
FileNode* find_child(FileNode* parent, const char* name);
void cmd_mkdir(const char* name);
void cmd_create(const char* name);
void cmd_write(const char* name, const char* content);
void cmd_read(const char* name);
void cmd_delete(const char* name);
void cmd_rmdir(const char* name);
void cmd_ls();
void cmd_cd(const char* path);
void cmd_pwd(FileNode* node);
void cmd_df();
void free_tree(FileNode* node);
void cmd_exit();
void get_prompt(char* buffer);

void init_vfs() {
    memset(virtualDisk, 0, sizeof(virtualDisk));
    
    for (int i = 0; i < NUM_BLOCKS; i++) {
        free_block_push_back(i);
    }
    
    root = create_node("/", NODE_DIR, NULL);
    cwd = root;
    used_blocks_count = 0;
}

void free_block_push_back(int index) {
    FreeBlock* new_block = (FreeBlock*)malloc(sizeof(FreeBlock));
    new_block->block_index = index;
    new_block->next = NULL;
    new_block->prev = NULL;

    if (freeListTail == NULL) {
        freeListHead = freeListTail = new_block;
    } else {
        freeListTail->next = new_block;
        new_block->prev = freeListTail;
        freeListTail = new_block;
    }
}

int free_block_pop_front() {
    if (freeListHead == NULL) return -1;
    
    FreeBlock* temp = freeListHead;
    int index = temp->block_index;
    
    freeListHead = freeListHead->next;
    if (freeListHead != NULL) {
        freeListHead->prev = NULL;
    } else {
        freeListTail = NULL;
    }
    
    free(temp);
    return index;
}

FileNode* create_node(const char* name, NodeType type, FileNode* parent) {
    FileNode* node = (FileNode*)malloc(sizeof(FileNode));
    strncpy(node->name, name, MAX_NAME - 1);
    node->name[MAX_NAME - 1] = '\0';
    node->type = type;
    node->parent = parent;
    node->child_head = NULL;
    node->block_pointers = NULL;
    node->num_blocks = 0;
    node->content_size = 0;
    node->next = node;
    node->prev = node;
    return node;
}

void add_child(FileNode* parent, FileNode* child) {
    if (parent->child_head == NULL) {
        parent->child_head = child;
        child->next = child;
        child->prev = child;
    } else {
        FileNode* tail = parent->child_head->prev;
        tail->next = child;
        child->prev = tail;
        child->next = parent->child_head;
        parent->child_head->prev = child;
    }
}

void remove_child(FileNode* parent, FileNode* child) {
    if (parent->child_head == child && child->next == child) {
        parent->child_head = NULL;
    } else {
        child->prev->next = child->next;
        child->next->prev = child->prev;
        if (parent->child_head == child) {
            parent->child_head = child->next;
        }
    }
    child->next = child->prev = child;
}

FileNode* find_child(FileNode* parent, const char* name) {
    if (parent->child_head == NULL) return NULL;
    FileNode* curr = parent->child_head;
    do {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    } while (curr != parent->child_head);
    return NULL;
}

void cmd_mkdir(const char* name) {
    if (strlen(name) == 0 || strlen(name) >= MAX_NAME) {
        printf("Error: Invalid directory name.\n");
        return;
    }
    if (find_child(cwd, name) != NULL) {
        printf("Error: Directory or file '%s' already exists.\n", name);
        return;
    }
    FileNode* new_dir = create_node(name, NODE_DIR, cwd);
    add_child(cwd, new_dir);
    printf("Directory '%s' created successfully.\n", name);
}

void cmd_create(const char* name) {
    if (strlen(name) == 0 || strlen(name) >= MAX_NAME) {
        printf("Error: Invalid file name.\n");
        return;
    }
    if (find_child(cwd, name) != NULL) {
        printf("Error: Directory or file '%s' already exists.\n", name);
        return;
    }
    FileNode* new_file = create_node(name, NODE_FILE, cwd);
    add_child(cwd, new_file);
    printf("File '%s' created successfully.\n", name);
}

void cmd_write(const char* name, const char* content) {
    FileNode* file = find_child(cwd, name);
    if (file == NULL) {
        printf("Error: File '%s' not found.\n", name);
        return;
    }
    if (file->type != NODE_FILE) {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }

    int len = strlen(content);
    int blocks_needed = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (blocks_needed == 0) blocks_needed = 1; 

    int free_count = 0;
    FreeBlock* curr = freeListHead;
    while (curr != NULL) {
        free_count++;
        curr = curr->next;
    }

    if (free_count < blocks_needed) {
        printf("Error: Disk full. Cannot write file (Required: %d blocks, Available: %d blocks).\n", blocks_needed, free_count);
        return;
    }

    if (file->num_blocks > 0) {
        for (int i = 0; i < file->num_blocks; i++) {
            memset(virtualDisk[file->block_pointers[i]], 0, BLOCK_SIZE);
            free_block_push_back(file->block_pointers[i]);
            used_blocks_count--;
        }
        free(file->block_pointers);
        file->block_pointers = NULL;
        file->num_blocks = 0;
    }

    file->block_pointers = (int*)malloc(blocks_needed * sizeof(int));
    file->num_blocks = blocks_needed;
    file->content_size = len;

    int bytes_written = 0;
    for (int i = 0; i < blocks_needed; i++) {
        int b_idx = free_block_pop_front();
        file->block_pointers[i] = b_idx;
        used_blocks_count++;

        int chunk_size = (len - bytes_written < BLOCK_SIZE) ? (len - bytes_written) : BLOCK_SIZE;
        if (chunk_size > 0) {
            memcpy(virtualDisk[b_idx], content + bytes_written, chunk_size);
            bytes_written += chunk_size;
        }
    }

    printf("Data written successfully (size=%d bytes).\n", len);
}

void cmd_read(const char* name) {
    FileNode* file = find_child(cwd, name);
    if (file == NULL) {
        printf("Error: File '%s' not found.\n", name);
        return;
    }
    if (file->type != NODE_FILE) {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }
    if (file->num_blocks == 0 || file->content_size == 0) {
        printf("(empty file)\n");
        return;
    }

    int bytes_remaining = file->content_size;
    for (int i = 0; i < file->num_blocks; i++) {
        int b_idx = file->block_pointers[i];
        int print_size = (bytes_remaining < BLOCK_SIZE) ? bytes_remaining : BLOCK_SIZE;
        fwrite(virtualDisk[b_idx], 1, print_size, stdout);
        bytes_remaining -= print_size;
    }
    printf("\n");
}

void cmd_delete(const char* name) {
    FileNode* file = find_child(cwd, name);
    if (file == NULL) {
        printf("Error: File '%s' not found.\n", name);
        return;
    }
    if (file->type != NODE_FILE) {
        printf("Error: '%s' is a directory. Use rmdir instead.\n", name);
        return;
    }

    for (int i = 0; i < file->num_blocks; i++) {
        memset(virtualDisk[file->block_pointers[i]], 0, BLOCK_SIZE);
        free_block_push_back(file->block_pointers[i]);
        used_blocks_count--;
    }
    if (file->block_pointers) free(file->block_pointers);

    remove_child(cwd, file);
    free(file);
    printf("File deleted successfully.\n");
}

void cmd_rmdir(const char* name) {
    FileNode* dir = find_child(cwd, name);
    if (dir == NULL) {
        printf("Error: Directory '%s' not found.\n", name);
        return;
    }
    if (dir->type != NODE_DIR) {
        printf("Error: '%s' is a file. Use delete instead.\n", name);
        return;
    }
    if (dir->child_head != NULL) {
        printf("Error: Directory '%s' is not empty.\n", name);
        return;
    }

    remove_child(cwd, dir);
    free(dir);
    printf("Directory removed successfully.\n");
}

void cmd_ls() {
    if (cwd->child_head == NULL) {
        printf("(empty)\n");
        return;
    }
    FileNode* curr = cwd->child_head;
    do {
        if (curr->type == NODE_DIR) {
            printf("[DIR]  %s/\n", curr->name);
        } else {
            printf("[FILE] %s (%d bytes)\n", curr->name, curr->content_size);
        }
        curr = curr->next;
    } while (curr != cwd->child_head);
}

void cmd_cd(const char* path) {
    if (strcmp(path, "..") == 0) {
        if (cwd->parent != NULL) {
            cwd = cwd->parent;
            printf("Moved to parent directory.\n");
        } else {
            printf("Already at root directory.\n");
        }
        return;
    }
    if (strcmp(path, "/") == 0) {
        cwd = root;
        printf("Moved to root directory.\n");
        return;
    }

    FileNode* target = find_child(cwd, path);
    if (target == NULL) {
        printf("Error: Directory '%s' not found.\n", path);
        return;
    }
    if (target->type != NODE_DIR) {
        printf("Error: '%s' is not a directory.\n", path);
        return;
    }
    cwd = target;
    printf("Moved to %s\n", path);
}

void cmd_pwd(FileNode* node) {
    if (node == NULL) return;
    if (node->parent == NULL) {
        printf("/");
        return;
    }
    cmd_pwd(node->parent);
    if (node->parent->parent != NULL) {
        printf("/");
    }
    printf("%s", node->name);
    if (node == cwd) {
        printf("\n");
    }
}

void cmd_df() {
    int free_blocks = NUM_BLOCKS - used_blocks_count;
    double usage_pct = ((double)used_blocks_count / NUM_BLOCKS) * 100.0;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", used_blocks_count);
    printf("Free Blocks: %d\n", free_blocks);
    printf("Disk Usage: %.2f%%\n", usage_pct);
}

void free_tree(FileNode* node) {
    if (node == NULL) return;
    if (node->type == NODE_DIR && node->child_head != NULL) {
        FileNode* curr = node->child_head;
        FileNode* start = curr;
        do {
            FileNode* next_node = curr->next;
            free_tree(curr);
            curr = next_node;
        } while (curr != start);
    }
    if (node->type == NODE_FILE) {
        if (node->block_pointers) free(node->block_pointers);
    }
    free(node);
}

void cmd_exit() {
    free_tree(root);

    FreeBlock* curr = freeListHead;
    while (curr != NULL) {
        FreeBlock* next = curr->next;
        free(curr);
        curr = next;
    }

    printf("Memory released. Exiting program...\n");
    exit(0);
}

void get_prompt(char* buffer) {
    if (cwd == root) {
        strcpy(buffer, "/ > ");
    } else {
        char path_stack[256] = "";
        FileNode* temp = cwd;
        while (temp != NULL && temp != root) {
            char temp_path[MAX_NAME + 256];
            snprintf(temp_path, sizeof(temp_path), "/%s%s", temp->name, path_stack);
            strcpy(path_stack, temp_path);
            temp = temp->parent;
        }
        snprintf(buffer, 256, "%s > ", path_stack);
    }
}

int main() {
    init_vfs();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char line[MAX_LINE];
    char prompt[256];

    while (1) {
        get_prompt(prompt);
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) break;

        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char cmd[32] = {0};
        char arg1[MAX_NAME] = {0};
        char arg2[MAX_LINE] = {0};

        if (strncmp(line, "write", 5) == 0) {
            char* first_space = strchr(line, ' ');
            if (!first_space) {
                printf("Usage: write <filename> \"content\"\n");
                continue;
            }
            sscanf(first_space + 1, "%s", arg1);
            char* quote_start = strchr(line, '"');
            if (quote_start) {
                char* quote_end = strrchr(line, '"');
                if (quote_end && quote_end > quote_start) {
                    *quote_end = '\0';
                    strcpy(arg2, quote_start + 1);
                } else {
                    strcpy(arg2, quote_start + 1);
                }
            } else {
                char* second_space = strchr(first_space + 1, ' ');
                if (second_space) {
                    strcpy(arg2, second_space + 1);
                }
            }
            cmd_write(arg1, arg2);
        } else {
            int scanned = sscanf(line, "%s %s", cmd, arg1);
            if (scanned < 1) continue;

            if (strcmp(cmd, "mkdir") == 0) {
                if (scanned < 2) printf("Usage: mkdir <dirname>\n");
                else cmd_mkdir(arg1);
            } else if (strcmp(cmd, "create") == 0) {
                if (scanned < 2) printf("Usage: create <filename>\n");
                else cmd_create(arg1);
            } else if (strcmp(cmd, "read") == 0) {
                if (scanned < 2) printf("Usage: read <filename>\n");
                else cmd_read(arg1);
            } else if (strcmp(cmd, "delete") == 0) {
                if (scanned < 2) printf("Usage: delete <filename>\n");
                else cmd_delete(arg1);
            } else if (strcmp(cmd, "rmdir") == 0) {
                if (scanned < 2) printf("Usage: rmdir <dirname>\n");
                else cmd_rmdir(arg1);
            } else if (strcmp(cmd, "ls") == 0) {
                cmd_ls();
            } else if (strcmp(cmd, "cd") == 0) {
                if (scanned < 2) printf("Usage: cd <path>\n");
                else cmd_cd(arg1);
            } else if (strcmp(cmd, "pwd") == 0) {
                cmd_pwd(cwd);
            } else if (strcmp(cmd, "df") == 0) {
                cmd_df();
            } else if (strcmp(cmd, "exit") == 0) {
                cmd_exit();
            } else {
                printf("Unknown command: %s\n", cmd);
            }
        }
    }

    return 0;
}