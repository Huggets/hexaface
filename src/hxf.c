#include "hxf.h"
#include <string.h>

// Used to find some errors with the allocations.
// For example, to verify hxfFree was called each time it was needed, you can verify at the end
// of the program (end of main.c) that allocCount is equal to 0. If that is not the case you can
// look at every HxfAlloc::ptr of allocsInfo and verify if they are equals to 0.
//
// You can also look how much memory is currently allocated each time hxfFree, hxfMalloc,
// hxfRealloc is called.
#if defined(HXF_DEBUG_ALLOC)
typedef struct HxfAlloc {
    void* ptr;
    size_t size;
} HxfAlloc;

/**
 * @brief Number of byte allocated at a given time
 */
static size_t byteAllocated = 0;

/**
 * @brief Index of the next HxfAlloc of allocsInfo that will be set.
 *
 * For now, it is never decremented and so the HxfAlloc that is freed will never be
 * redefined at a later time.
 * TODO fix this.
 */
static size_t allocIndex = 0;

/**
 * @brief The number of allocation made at a given time.
 */
static size_t allocCount = 0;

/**
 * @brief Information on each allocation made.
 */
static HxfAlloc allocsInfo[1024] = { 0 };
#endif

void* hxfMalloc(size_t size) {
    void* data = malloc(size);

    if (data == NULL) {
        HXF_MSG_ERROR("Could not allocate memory");
        exit(EXIT_FAILURE);
    }

#if defined(HXF_DEBUG_ALLOC)
    // Set a new HxfAlloc
    allocsInfo[allocIndex].ptr = data;
    allocsInfo[allocIndex].size = size;

    byteAllocated += size; // Update the value
    allocIndex++;
    allocCount++;
    printf("malloc: new size %lluB, malloc count = %llu\n", byteAllocated, allocCount);
#endif

    return data;
}

void* hxfRealloc(void* ptr, size_t size) {
    void* data = realloc(ptr, size);

    if (data == NULL) {
        HXF_MSG_ERROR("Could not reallocate memory");
        exit(EXIT_FAILURE);
    }

#if defined(HXF_DEBUG_ALLOC)
    for (int i = allocIndex - 1; i != -1; i--) {
        if (allocsInfo[i].ptr == ptr) {
            // Redefine the HxfAlloc
            allocsInfo[i].ptr = data;
            byteAllocated = byteAllocated - allocsInfo[i].size + size; // Update
            allocsInfo[i].size = size;

            printf("realloc: new size %lluB, malloc count = %llu\n", byteAllocated, allocCount);
            break;
        }
}
#endif

    return data;
}

void hxfFree(void* ptr) {
    free(ptr);

#if defined(HXF_DEBUG_ALLOC)
    for (int i = allocIndex - 1; i != -1; i--) {
        if (allocsInfo[i].ptr == ptr) {
            byteAllocated -= allocsInfo[i].size; // Update

            allocsInfo[i].ptr = NULL; // Clear the ptr value
            break;
        }
    }
    allocCount--;
    printf("free: new size %lluB, malloc count = %llu\n", byteAllocated, allocCount);
#endif
}

HxfResult hxfReadFile(const char* filename, void** data, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return HXF_ERROR;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate and read the file
    *data = hxfMalloc(*size);
    fread(*data, *size, 1, file);

    return HXF_SUCCESS;
}