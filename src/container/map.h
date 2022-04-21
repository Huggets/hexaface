#pragma once

#include <stdint.h>

/**
 * @brief A hash map.
 *
 * No elements can have the same key. If so, then the previous value is replaced
 * by the new value.
 *
 * A hash function is used to compute the index inside the table.
 * The table elements are a pointer to their data.
 */
typedef struct HxfHashMap {
    /**
     * @brief The hash table
     *
     * It’s a pointer to the hash map data that contains the key/value pairs.
     */
    void** table;
    /**
     * @brief A pointer to the hash function used in the hash map.
     *
     * @return The index associated with the key in the hash map.
     */
    uint32_t(*hash)(const void* restrict key);
} HxfHashMap;

/**
 * @brief Get the value associated with the given key.
 *
 * @param map The map to search in. Must not be NULL.
 * @param key The key of the element. Must not be NULL.
 *
 * @return The value associated with the key.
 */
void* hxfHashMapGet(const HxfHashMap* restrict map, const void* restrict key);

/**
 * @brief Get the value associated with the given hash.
 *
 * This works the same as hxfHashMapGet but with the hash already computed
 * and replacing the key.
 *
 * @param map The map to search in. Must ont be NULL.
 * @param hash The hash of the key.
 *
 * @return The value associated with the hash.
 */
void* hxfHashMapGetFromHash(const HxfHashMap* restrict map, uint32_t hash);

/**
 * @brief Put the value with the given key in the hash map.
 *
 * @param map The map that will hold the value. Must not be NULL.
 * @param key The key of the element. Must not be NULL.
 * @param value The value of the element.
 */
void hxfHashMapPut(HxfHashMap* restrict map, const void* restrict key, void* value);

/**
 * @brief Put the value associated with the given hash in the hash map.
 *
 * This is the same as hxfHashMapPut but with the hash already computed and
 * replacing the key.
 *
 * @param map The map that will hold the value. Must not be NULL.
 * @param key The hash of the element. Must not be NULL.
 * @param value The value of the element.
 */
void hxfHashMapPutFromHash(HxfHashMap* restrict map, uint32_t hash, void* value);
