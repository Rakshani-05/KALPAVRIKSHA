#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define MAX_POOL_SIZE 1000000

typedef struct {
    size_t start;
    size_t size;
    long long id;
    int allocated;
} Block;

Block *blocks = NULL;
unsigned char *memoryPool = NULL;

size_t poolSize;
size_t blockCount = 0;
size_t capacity = 0;

long long nextID = 1;
int strategy = 1;

int readInput(const char *message, long long *value) {
    char input[100];
    char *end;

    printf("%s", message);

    if (fgets(input, sizeof(input), stdin) == NULL)
        return 0;

    long long number = strtoll(input, &end, 10);

    if (end == input)
        return 0;

    while (*end == ' ' || *end == '\t' ||
           *end == '\n' || *end == '\r')
        end++;

    if (*end != '\0')
        return 0;

    *value = number;
    return 1;
}

int ensureCapacity(size_t required) {
    if (required <= capacity)
        return 1;

    size_t newCapacity = capacity ? capacity : 4;

    while (newCapacity < required) {
        if (newCapacity > SIZE_MAX / 2) {
            newCapacity = required;
            break;
        }

        newCapacity *= 2;
    }

    if (newCapacity > SIZE_MAX / sizeof(Block))
        return 0;

    Block *temp = realloc(blocks, newCapacity * sizeof(Block));

    if (temp == NULL)
        return 0;

    blocks = temp;
    capacity = newCapacity;

    return 1;
}

int findBlock(size_t request) {
    int selected = -1;

    for (size_t i = 0; i < blockCount; i++) {
        if ((* (blocks + i)).allocated ||
            (* (blocks + i)).size < request)
            continue;

        if (strategy == 1)
            return (int)i;

        if (selected == -1)
            selected = (int)i;

        else if (strategy == 2 &&
                 (* (blocks + i)).size <
                 (* (blocks + selected)).size)
            selected = (int)i;

        else if (strategy == 3 &&
                 (* (blocks + i)).size >
                 (* (blocks + selected)).size)
            selected = (int)i;
    }

    return selected;
}

int allocateAt(size_t index, size_t request, long long id) {
    size_t remaining = (*(blocks + index)).size - request;
    size_t start = (*(blocks + index)).start;

    if (remaining > 0) {
        memmove(blocks + index + 2,
                blocks + index + 1,
                (blockCount - index - 1) * sizeof(Block));

        (*(blocks + index + 1)).start = start + request;
        (*(blocks + index + 1)).size = remaining;
        (*(blocks + index + 1)).id = 0;
        (*(blocks + index + 1)).allocated = 0;

        blockCount++;
    }

    (*(blocks + index)).start = start;
    (*(blocks + index)).size = request;
    (*(blocks + index)).id = id;
    (*(blocks + index)).allocated = 1;

    return 1;
}

void mergeBlocks(void) {
    size_t i = 0;

    while (i + 1 < blockCount) {
        if (!(*(blocks + i)).allocated &&
            !(*(blocks + i + 1)).allocated) {

            (*(blocks + i)).size +=
                (*(blocks + i + 1)).size;

            memmove(blocks + i + 1,
                    blocks + i + 2,
                    (blockCount - i - 2) * sizeof(Block));

            blockCount--;
        }
        else {
            i++;
        }
    }
}

void allocateMemory(void) {
    long long input;

    if (!readInput("Enter requested size: ", &input)) {
        printf("Invalid input.\n");
        return;
    }

    if (input <= 0 || (unsigned long long)input > poolSize) {
        printf("Invalid allocation size.\n");
        return;
    }

    if (nextID == LLONG_MAX) {
        printf("Allocation ID limit reached.\n");
        return;
    }

    size_t request = (size_t)input;

    int index = findBlock(request);

    if (index == -1) {
        printf("Allocation failed.\n");
        detectFragmentation(request);
        return;
    }

    if (!ensureCapacity(blockCount + 1)) {
        printf("Metadata allocation failed.\n");
        return;
    }

    long long id = nextID;

    allocateAt((size_t)index, request, id);
    nextID++;

    printf("Allocation successful.\n");
    printf("ID: %lld\n", id);
    printf("Start: %zu\n", (*(blocks + index)).start);
    printf("Size: %zu bytes\n", request);
}

void releaseMemory(void) {
    long long id;

    if (!readInput("Enter allocation ID: ", &id)) {
        printf("Invalid input.\n");
        return;
    }

    if (id <= 0) {
        printf("Invalid allocation ID.\n");
        return;
    }

    for (size_t i = 0; i < blockCount; i++) {
        if ((*(blocks + i)).allocated &&
            (*(blocks + i)).id == id) {

            (*(blocks + i)).allocated = 0;
            (*(blocks + i)).id = 0;

            mergeBlocks();

            printf("Allocation released successfully.\n");
            return;
        }
    }

    printf("Invalid allocation ID.\n");
}

void resizeMemory(void) {
    long long id, input;

    if (!readInput("Enter allocation ID: ", &id)) {
        printf("Invalid input.\n");
        return;
    }

    if (id <= 0) {
        printf("Invalid allocation ID.\n");
        return;
    }

    size_t index = blockCount;

    for (size_t i = 0; i < blockCount; i++) {
        if ((*(blocks + i)).allocated &&
            (*(blocks + i)).id == id) {
            index = i;
            break;
        }
    }

    if (index == blockCount) {
        printf("Allocation not found.\n");
        return;
    }

    if (!readInput("Enter new size: ", &input)) {
        printf("Invalid input.\n");
        return;
    }

    if (input <= 0 || (unsigned long long)input > poolSize) {
        printf("Invalid size.\n");
        return;
    }

    size_t newSize = (size_t)input;
    size_t oldSize = (*(blocks + index)).size;
    size_t oldStart = (*(blocks + index)).start;

    if (newSize == oldSize) {
        printf("Size unchanged.\n");
        return;
    }

    if (newSize < oldSize) {
        if (!ensureCapacity(blockCount + 1)) {
            printf("Metadata allocation failed.\n");
            return;
        }

        memmove(blocks + index + 2,
                blocks + index + 1,
                (blockCount - index - 1) * sizeof(Block));

        (*(blocks + index)).size = newSize;

        (*(blocks + index + 1)).start = oldStart + newSize;
        (*(blocks + index + 1)).size = oldSize - newSize;
        (*(blocks + index + 1)).id = 0;
        (*(blocks + index + 1)).allocated = 0;

        blockCount++;

        mergeBlocks();

        printf("Allocation shrunk successfully.\n");
        return;
    }

    size_t extra = newSize - oldSize;

    if (index + 1 < blockCount &&
        !(*(blocks + index + 1)).allocated &&
        (*(blocks + index + 1)).size >= extra) {

        if ((*(blocks + index + 1)).size == extra) {
            memmove(blocks + index + 1,
                    blocks + index + 2,
                    (blockCount - index - 2) * sizeof(Block));

            blockCount--;
        }
        else {
            (*(blocks + index + 1)).start += extra;
            (*(blocks + index + 1)).size -= extra;
        }

        (*(blocks + index)).size = newSize;

        printf("Allocation expanded in place.\n");
        return;
    }

    int newIndex = findBlock(newSize);

    if (newIndex == -1) {
        printf("Resize failed.\n");
        detectFragmentation(newSize);
        return;
    }

    if (!ensureCapacity(blockCount + 1)) {
        printf("Metadata allocation failed.\n");
        return;
    }

    size_t newStart = (*(blocks + newIndex)).start;

    memmove(memoryPool + newStart,
            memoryPool + oldStart,
            oldSize);

    allocateAt((size_t)newIndex, newSize, id);

    for (size_t i = 0; i < blockCount; i++) {
        if ((*(blocks + i)).allocated &&
            (*(blocks + i)).id == id &&
            (*(blocks + i)).start == oldStart) {

            (*(blocks + i)).allocated = 0;
            (*(blocks + i)).id = 0;
            break;
        }
    }

    mergeBlocks();

    printf("Allocation moved and resized successfully.\n");
    printf("New Start: %zu\n", newStart);
    printf("New Size: %zu bytes\n", newSize);
}

void displayMemoryMap(void) {
    printf("\nMemory Map (%zu bytes):\n", poolSize);

    for (size_t i = 0; i < blockCount; i++) {
        if ((*(blocks + i)).allocated) {
            printf("[Allocated #%lld: %zu bytes, Start: %zu]\n",
                   (*(blocks + i)).id,
                   (*(blocks + i)).size,
                   (*(blocks + i)).start);
        }
        else {
            printf("[Free: %zu bytes, Start: %zu]\n",
                   (*(blocks + i)).size,
                   (*(blocks + i)).start);
        }
    }
}

void displayStatistics(void) {
    size_t allocated = 0;
    size_t freeMemory = 0;
    size_t allocatedBlocks = 0;
    size_t freeBlocks = 0;
    size_t largestFree = 0;

    for (size_t i = 0; i < blockCount; i++) {
        if ((*(blocks + i)).allocated) {
            allocated += (*(blocks + i)).size;
            allocatedBlocks++;
        }
        else {
            freeMemory += (*(blocks + i)).size;
            freeBlocks++;

            if ((*(blocks + i)).size > largestFree)
                largestFree = (*(blocks + i)).size;
        }
    }

    printf("\nMemory Statistics\n");
    printf("Total memory: %zu bytes\n", poolSize);
    printf("Allocated: %zu bytes\n", allocated);
    printf("Free: %zu bytes\n", freeMemory);
    printf("Allocated blocks: %zu\n", allocatedBlocks);
    printf("Free blocks: %zu\n", freeBlocks);
    printf("Largest free block: %zu bytes\n", largestFree);
}

void detectFragmentation(size_t request) {
    size_t totalFree = 0;
    size_t largestFree = 0;

    for (size_t i = 0; i < blockCount; i++) {
        if (!(*(blocks + i)).allocated) {
            totalFree += (*(blocks + i)).size;

            if ((*(blocks + i)).size > largestFree)
                largestFree = (*(blocks + i)).size;
        }
    }

    if (totalFree >= request && largestFree < request) {
        printf("\nExternal Fragmentation Detected!\n");
        printf("Requested: %zu bytes\n", request);
        printf("Total Free: %zu bytes\n", totalFree);
        printf("Largest Free Block: %zu bytes\n", largestFree);

        printf("Individual Free Blocks: ");

        for (size_t i = 0; i < blockCount; i++) {
            if (!(*(blocks + i)).allocated)
                printf("%zu ", (*(blocks + i)).size);
        }

        printf("\n");
    }
    else {
        printf("Insufficient total free memory.\n");
    }
}

void changeStrategy(void) {
    long long choice;

    printf("\n1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    if (!readInput("Select strategy: ", &choice)) {
        printf("Invalid input.\n");
        return;
    }

    if (choice < 1 || choice > 3) {
        printf("Invalid strategy.\n");
        return;
    }

    strategy = (int)choice;

    if (strategy == 1)
        printf("Strategy changed to First Fit.\n");
    else if (strategy == 2)
        printf("Strategy changed to Best Fit.\n");
    else
        printf("Strategy changed to Worst Fit.\n");
}

void cleanup(void) {
    free(memoryPool);
    free(blocks);

    memoryPool = NULL;
    blocks = NULL;
}

int main(void) {
    long long input;

    printf("SMART MEMORY MANAGER\n");

    if (!readInput("Enter memory pool size: ", &input))
        return 1;

    if (input < 1 || input > MAX_POOL_SIZE) {
        printf("Invalid pool size.\n");
        return 1;
    }

    poolSize = (size_t)input;

    memoryPool = malloc(poolSize);

    if (memoryPool == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    capacity = 4;
    blocks = malloc(capacity * sizeof(Block));

    if (blocks == NULL) {
        printf("Metadata allocation failed.\n");
        cleanup();
        return 1;
    }

    blockCount = 1;

    (*(blocks + 0)).start = 0;
    (*(blocks + 0)).size = poolSize;
    (*(blocks + 0)).id = 0;
    (*(blocks + 0)).allocated = 0;

    printf("\n1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    if (!readInput("Select allocation strategy: ", &input) ||
        input < 1 || input > 3) {
        printf("Invalid strategy.\n");
        cleanup();
        return 1;
    }

    strategy = (int)input;

    int running = 1;

    while (running) {
        printf("\nMENU\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");

        if (!readInput("Choice: ", &input)) {
            printf("Invalid input.\n");
            continue;
        }

        switch (input) {
            case 1:
                allocateMemory();
                break;

            case 2:
                releaseMemory();
                break;

            case 3:
                resizeMemory();
                break;

            case 4:
                displayMemoryMap();
                break;

            case 5:
                displayStatistics();
                break;

            case 6:
                changeStrategy();
                break;

            case 7:
                running = 0;
                break;

            default:
                printf("Invalid choice.\n");
        }
    }

    cleanup();

    printf("Memory manager terminated.\n");

    return 0;
}
