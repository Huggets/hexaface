#include "hxf.h"

void * hxfMalloc(size_t size) {
    void * data = malloc(size);
    if (data == NULL) {
        HXF_MSG_ERROR("Could not allocate memory");
        exit(EXIT_FAILURE);
    }

    return data;
}

HxfResult readFile(char * filename, void ** data, size_t * size) {
    // TODO when this program will be ported on windows, test if the "b" flag need to be added
    FILE * file = fopen(filename, "r");
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