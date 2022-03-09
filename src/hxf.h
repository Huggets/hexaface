/**
 * \file hxf.h
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>

#define HXF_MSG_ERROR(msg, ...) fprintf(stderr, "error: "msg"\n",##__VA_ARGS__);

 /**
  * \enum HxfResult
  * \brief A result a function returns.
  */
typedef enum HxfResult {
    HXF_SUCCESS, ///< No error
    HXF_ERROR, ///< General purpose error
    HXF_WINDOW_CREATION_ERROR ///< Error when creating a window
} HxfResult;

/**
 * \brief Allocate size bytes of memory.
 * \param size The size of the block of memory to allocate.
 * \return A pointer to the data allocated.
 *
 * If the allocation fails, then it exits the program.
 */
void * hxfMalloc(size_t size);

/**
 * \brief Read a file.
 *
 * Read the filename and put all the content into data.
 * 
 * data is allocated and thus, must be freed.
 * 
 * \param filename The name of the file.
 * \param data A pointer that will be allocated and that will contain the content of the file.\n
 * It must be freed when not used anymore.
 * \param size A pointer that will contain the size of the file.
 * 
 * \return HXF_ERROR if the file could not be open, HXF_SUCCESS otherwise.
 */
HxfResult readFile(const char * filename, void ** data, size_t * size);