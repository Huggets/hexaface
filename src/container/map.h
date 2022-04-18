#pragma once

/**
 * @brief Represent a map but also a map element.
 */
typedef struct HxfMapElement {
    void* key; ///< The key of the element. It must not be NULL.
    void* value; ///< A pointer to the value of the element.
    struct HxfMapElement* next; ///< A pointer to the address of the next element. NULL if no next element.
} HxfMapElement;

typedef struct HxfMap {
    HxfMapElement* start; ///< A pointer to the first element of the map.
    /**
     * @brief The function called to test if two keys are equals.
     *
     * This is used in the maps function to select the right element.
     */
    int (*compareKey)(const void*, const void*);
} HxfMap;

/**
 * @brief Get the map element associated with the given key.
 *
 * @param map The map to search in. Must not be NULL.
 * @param key The key of the element. Must not be NULL.
 *
 * @return A pointer to the map element or NULL if it was not found.
 */
HxfMapElement* hxfMapGet(const HxfMap* map, const void* key);

/**
 * @brief Set the value of the element with the given key.
 *
 * Create the element if it does not exists. Otherwise update the value.
 *
 * @param map The map that will hold the value. Must not be NULL.
 * @param key The key of the element. Must not be NULL.
 * @param value The value of the element.
 */
void hxfMapSet(HxfMap* map, void* key, void* value);

/**
 * @brief Remove the element of the map.
 *
 * The element must exist in the map.
 *
 * @param map The map where the element is.
 * @param key The key of the element.
 */
void hxfMapRemove(HxfMap* map, const void* key);