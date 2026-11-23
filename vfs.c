#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

typedef struct FreeBlock {
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME + 1];
    int isDirectory;
    struct FileNode *parent;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *children;
    int *blockPointers;
    int blockCount;
    size_t contentSize;
} FileNode;

FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;

FileNode *root = NULL;
FileNode *cwd = NULL;

int freeCount = 0;
int usedCount = 0;

void initializeFreeBlocks() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        FreeBlock *newBlock = malloc(sizeof(FreeBlock));

        if (newBlock == NULL) {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        newBlock->index = i;
        newBlock->next = NULL;
        newBlock->prev = freeTail;

        if (freeHead == NULL) {
            freeHead = newBlock;
        } else {
            freeTail->next = newBlock;
        }

        freeTail = newBlock;
        freeCount++;
    }
}

int allocateBlock() {
    if (freeHead == NULL) {
        return -1;
    }

    FreeBlock *temp = freeHead;
    int index = temp->index;

    freeHead = freeHead->next;

    if (freeHead != NULL) {
        freeHead->prev = NULL;
    } else {
        freeTail = NULL;
    }

    free(temp);

    freeCount--;
    usedCount++;

    return index;
}

int releaseBlock(int index) {
    FreeBlock *newBlock = malloc(sizeof(FreeBlock));

    if (newBlock == NULL) {
        return 0;
    }

    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = freeTail;

    if (freeTail == NULL) {
        freeHead = newBlock;
    } else {
        freeTail->next = newBlock;
    }

    freeTail = newBlock;

    freeCount++;
    usedCount--;

    return 1;
}

FileNode *createNode(const char *name, int isDirectory) {
    FileNode *node = calloc(1, sizeof(FileNode));

    if (node == NULL) {
        return NULL;
    }

    strncpy(node->name, name, MAX_NAME);
    node->name[MAX_NAME] = '\0';

    node->isDirectory = isDirectory;
    node->next = node;
    node->prev = node;

    return node;
}

FileNode *findEntry(FileNode *directory, const char *name) {
    if (directory == NULL || directory->children == NULL) {
        return NULL;
    }

    FileNode *current = directory->children;

    do {
        if (strcmp(current->name, name) == 0) {
            return current;
        }

        current = current->next;

    } while (current != directory->children);

    return NULL;
}

void insertNode(FileNode *directory, FileNode *node) {
    node->parent = directory;

    if (directory->children == NULL) {
        node->next = node;
        node->prev = node;
        directory->children = node;
        return;
    }

    FileNode *last = directory->children->prev;

    last->next = node;
    node->prev = last;

    node->next = directory->children;
    directory->children->prev = node;
}

void removeNode(FileNode *node) {
    FileNode *parent = node->parent;

    if (node->next == node) {
        parent->children = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (parent->children == node) {
            parent->children = node->next;
        }
    }

    node->next = node;
    node->prev = node;
    node->parent = NULL;
}

void printPathRecursive(FileNode *directory) {
    if (directory == NULL) {
        return;
    }

    if (directory == root) {
        printf("/");
        return;
    }

    printPathRecursive(directory->parent);

    if (directory->parent != root) {
        printf("/");
    }

    printf("%s", directory->name);
}

void pwd() {
    printPathRecursive(cwd);
    printf("\n");
}

void mkdirCommand(char *name) {
    if (name == NULL || strlen(name) == 0 ||
        strlen(name) > MAX_NAME) {
        printf("Invalid directory name.\n");
        return;
    }

    if (findEntry(cwd, name) != NULL) {
        printf("Entry already exists.\n");
        return;
    }

    FileNode *directory = createNode(name, 1);

    if (directory == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    insertNode(cwd, directory);

    printf("Directory '%s' created successfully.\n", name);
}

void createCommand(char *name) {
    if (name == NULL || strlen(name) == 0 ||
        strlen(name) > MAX_NAME) {
        printf("Invalid filename.\n");
        return;
    }

    if (findEntry(cwd, name) != NULL) {
        printf("Entry already exists.\n");
        return;
    }

    FileNode *file = createNode(name, 0);

    if (file == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    insertNode(cwd, file);

    printf("File '%s' created successfully.\n", name);
}

void writeCommand(char *name, char *content) {
    if (name == NULL || content == NULL) {
        printf("Usage: write filename \"content\"\n");
        return;
    }

    FileNode *file = findEntry(cwd, name);

    if (file == NULL || file->isDirectory) {
        printf("File not found.\n");
        return;
    }

    size_t length = strlen(content);

    if (length >= 2 &&
        ((content[0] == '"' && content[length - 1] == '"') ||
         (content[0] == '\'' && content[length - 1] == '\''))) {

        content[length - 1] = '\0';
        content++;
    }

    size_t contentLength = strlen(content);

    int requiredBlocks =
        (int)((contentLength + BLOCK_SIZE - 1) / BLOCK_SIZE);

    if (requiredBlocks > freeCount + file->blockCount) {
        printf("Disk full. Not enough free blocks.\n");
        return;
    }

    int *newPointers = NULL;

    if (requiredBlocks > 0) {
        newPointers = malloc(requiredBlocks * sizeof(int));

        if (newPointers == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
    }

    for (int i = 0; i < file->blockCount; i++) {
        if (!releaseBlock(file->blockPointers[i])) {
            free(newPointers);
            printf("Unable to release old blocks. Write cancelled.\n");
            return;
        }
    }

    free(file->blockPointers);

    file->blockPointers = newPointers;
    file->blockCount = 0;
    file->contentSize = 0;

    for (int i = 0; i < requiredBlocks; i++) {
        int block = allocateBlock();

        if (block == -1) {
            printf("Block allocation failed.\n");
            return;
        }

        file->blockPointers[i] = block;
        file->blockCount++;
    }

    size_t written = 0;

    for (int i = 0; i < requiredBlocks; i++) {
        int block = file->blockPointers[i];

        size_t remaining = contentLength - written;

        size_t bytes =
            remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        memcpy(virtualDisk[block], content + written, bytes);

        written += bytes;
    }

    file->contentSize = contentLength;

    printf("Data written successfully (size=%zu bytes).\n",
           contentLength);
}

void readCommand(char *name) {
    if (name == NULL) {
        printf("Usage: read filename\n");
        return;
    }

    FileNode *file = findEntry(cwd, name);

    if (file == NULL || file->isDirectory) {
        printf("File not found.\n");
        return;
    }

    if (file->contentSize == 0) {
        printf("File is empty.\n");
        return;
    }

    size_t remaining = file->contentSize;

    for (int i = 0; i < file->blockCount; i++) {
        int block = file->blockPointers[i];

        size_t bytes =
            remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        fwrite(virtualDisk[block], 1, bytes, stdout);

        remaining -= bytes;
    }

    printf("\n");
}

void deleteCommand(char *name) {
    if (name == NULL) {
        printf("Usage: delete filename\n");
        return;
    }

    FileNode *file = findEntry(cwd, name);

    if (file == NULL || file->isDirectory) {
        printf("File not found.\n");
        return;
    }

    for (int i = 0; i < file->blockCount; i++) {
        if (!releaseBlock(file->blockPointers[i])) {
            printf("Unable to release all blocks. Deletion cancelled.\n");
            return;
        }
    }

    free(file->blockPointers);

    removeNode(file);
    free(file);

    printf("File deleted successfully.\n");
}

void rmdirCommand(char *name) {
    if (name == NULL) {
        printf("Usage: rmdir directory\n");
        return;
    }

    FileNode *directory = findEntry(cwd, name);

    if (directory == NULL || !directory->isDirectory) {
        printf("Directory not found.\n");
        return;
    }

    if (directory->children != NULL) {
        printf("Directory is not empty.\n");
        return;
    }

    removeNode(directory);
    free(directory);

    printf("Directory removed successfully.\n");
}

void lsCommand(char *name) {
    FileNode *directory = cwd;

    if (name != NULL) {
        if (strcmp(name, "/") == 0) {
            directory = root;
        } else {
            directory = findEntry(cwd, name);

            if (directory == NULL || !directory->isDirectory) {
                printf("Directory not found.\n");
                return;
            }
        }
    }

    if (directory->children == NULL) {
        printf("(empty)\n");
        return;
    }

    FileNode *current = directory->children;

    do {
        if (current->isDirectory) {
            printf("[DIR]  %s\n", current->name);
        } else {
            printf("[FILE] %s\n", current->name);
        }

        current = current->next;

    } while (current != directory->children);
}

void cdCommand(char *name) {
    if (name == NULL) {
        printf("Usage: cd directory\n");
        return;
    }

    if (strcmp(name, "/") == 0) {
        cwd = root;
        printf("Moved to /\n");
        return;
    }

    if (strcmp(name, "..") == 0) {
        if (cwd->parent != NULL) {
            cwd = cwd->parent;
        }

        printf("Moved to ");
        pwd();
        return;
    }

    FileNode *directory = findEntry(cwd, name);

    if (directory == NULL || !directory->isDirectory) {
        printf("Directory not found.\n");
        return;
    }

    cwd = directory;

    printf("Moved to ");
    pwd();
}

void dfCommand() {
    printf("\nTotal Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedCount);
    printf("Free Blocks: %d\n", freeCount);

    double usage =
        ((double)usedCount / NUM_BLOCKS) * 100.0;

    printf("Disk Usage: %.2f%%\n\n", usage);
}

void freeDirectory(FileNode *directory) {
    if (directory == NULL) {
        return;
    }

    while (directory->children != NULL) {
        FileNode *child = directory->children;

        if (child->isDirectory) {
            freeDirectory(child);
        } else {
            free(child->blockPointers);
        }

        removeNode(child);
        free(child);
    }
}

void cleanup() {
    freeDirectory(root);

    free(root);

    root = NULL;
    cwd = NULL;

    FreeBlock *current = freeHead;

    while (current != NULL) {
        FreeBlock *temp = current;
        current = current->next;
        free(temp);
    }

    freeHead = NULL;
    freeTail = NULL;

    printf("Memory released. Exiting program...\n");
}

void initializeVFS() {
    initializeFreeBlocks();

    root = createNode("/", 1);

    if (root == NULL) {
        printf("Root initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    cwd = root;
}

int main() {
    initializeVFS();

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char input[4096];

    while (1) {
        printPathRecursive(cwd);
        printf(" > ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        char *command = input;

        while (isspace((unsigned char)*command)) {
            command++;
        }

        if (*command == '\0') {
            continue;
        }

        char *args = command;

        while (*args && !isspace((unsigned char)*args)) {
            args++;
        }

        if (*args != '\0') {
            *args = '\0';
            args++;

            while (isspace((unsigned char)*args)) {
                args++;
            }
        } else {
            args = NULL;
        }

        if (strcmp(command, "mkdir") == 0) {
            mkdirCommand(args);

        } else if (strcmp(command, "create") == 0) {
            createCommand(args);

        } else if (strcmp(command, "write") == 0) {
            if (args == NULL) {
                printf("Usage: write filename \"content\"\n");
                continue;
            }

            char *name = args;

            while (*args && !isspace((unsigned char)*args)) {
                args++;
            }

            char *content = NULL;

            if (*args != '\0') {
                *args = '\0';
                args++;

                while (isspace((unsigned char)*args)) {
                    args++;
                }

                if (*args != '\0') {
                    content = args;
                }
            }

            writeCommand(name, content);

        } else if (strcmp(command, "read") == 0) {
            readCommand(args);

        } else if (strcmp(command, "delete") == 0) {
            deleteCommand(args);

        } else if (strcmp(command, "rmdir") == 0) {
            rmdirCommand(args);

        } else if (strcmp(command, "ls") == 0) {
            lsCommand(args);

        } else if (strcmp(command, "cd") == 0) {
            cdCommand(args);

        } else if (strcmp(command, "pwd") == 0) {
            pwd();

        } else if (strcmp(command, "df") == 0) {
            dfCommand();

        } else if (strcmp(command, "exit") == 0) {
            cleanup();
            return 0;

        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    cleanup();

    return 0;
}