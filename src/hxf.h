/**
 * \file hxf.h
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Print an error message to the error output.
 */
#define HXF_MSG_ERROR(msg, ...) fprintf(stderr, "error: "msg"\n",##__VA_ARGS__);
/**
 * @brief Print an error message to the error output and exit with an EXIT_FAILURE status.
 */
#define HXF_FATAL(msg, ...) fprintf(stderr, "fatal error: "msg"\n",##__VA_ARGS__); exit(EXIT_FAILURE);

#define HXF_TRY_VK(funcCall) if (funcCall != VK_SUCCESS) { fprintf(stderr, "runtime error in file %s at line %i", __FILE__, __LINE__); exit(EXIT_FAILURE); }

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
 * @brief A custom call to malloc.
 *
 * Exit the progam if it fails.
 *
 * @param size The number of byte to allocated.
 *
 * @return The return value of malloc.
 */
void* hxfMalloc(size_t size);

/**
 * @brief A custom call to calloc.
 *
 * Exit the program if it fails.
 *
 * @param num The number of element to allocate.
 * @param size The size of one element.
 *
 * @return The return value of calloc.
 */
void* hxfCalloc(size_t num, size_t size);

/**
 * @brief Reallocate ptr with its new size.
 *
 * If it fails, it exits the program.
 */
void* hxfRealloc(void* ptr, size_t size);

/**
 * @brief Free the memory.
 *
 * @param ptr
 */
void hxfFree(void* ptr);

/**
 * @brief Print the information about the allocation if
 * the HXF_DEBUG_ALLOC macro is defined.
 */
void hxfAllocationInfo();

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
HxfResult hxfReadFile(const char* filename, void** data, size_t* size);
