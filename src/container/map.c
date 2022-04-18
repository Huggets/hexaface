#include "map.h"
#include "../hxf.h"
#include <stdlib.h>

HxfMapElement* hxfMapGet(const HxfMap* map, const void* key) {
    HxfMapElement* iterator = map->start;

    while (iterator != NULL && !map->compareKey(iterator->key, key)) {
        iterator = iterator->next;
    }

    return iterator;
}

void hxfMapSet(HxfMap* map, void* key, void* value) {
    // Set the first element if the map is empty.

    if (map->start == NULL) {
        HxfMapElement* newElement = hxfMalloc(sizeof(HxfMapElement));
        newElement->key = key;
        newElement->value = value;
        newElement->next = NULL;
        map->start = newElement;
    }
    else {
        // Create a new element if the key is not found.
        // Otherwise update the element with the same key.

        HxfMapElement* iterator = map->start;
        int differentKey = !map->compareKey(iterator->key, key);

        while (iterator->next != NULL && differentKey) {
            iterator = iterator->next;
        }

        if (differentKey) {
            HxfMapElement* newElement = hxfMalloc(sizeof(HxfMapElement));
            newElement->key = key;
            newElement->value = value;
            newElement->next = NULL;
            iterator->next = newElement;
        }
        else {
            iterator->value = value;
        }
    }
}

void hxfMapRemove(HxfMap* map, const void* key) {
    HxfMapElement* iterator = map->start;

    if (map->compareKey(iterator->key, key)) { // This is the first element of the map
        if (iterator->next == NULL) { // This is also the last element
            hxfFree(iterator);
            map->start = NULL;
        }
        else {
            HxfMapElement* toDelete = iterator->next;

            iterator->key = iterator->next->key;
            iterator->value = iterator->next->value;
            iterator->next = iterator->next->next;

            hxfFree(toDelete);
        }
    }
    else {
        HxfMapElement* nextIterator = iterator->next;

        while (iterator != NULL && !map->compareKey(nextIterator->key, key)) {
            iterator = nextIterator;
            nextIterator = iterator->next;
        }

        if (nextIterator->next == NULL) { // The element is the last element of the map
            iterator->next = NULL;
            hxfFree(nextIterator);
        }
        else {
            HxfMapElement* toDelete = nextIterator->next;
            nextIterator->key = toDelete->key;
            nextIterator->value = toDelete->value;
            nextIterator->next = toDelete->next;
            hxfFree(toDelete);
        }
    }
}