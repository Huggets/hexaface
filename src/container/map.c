#include "map.h"

void* hxfHashMapGet(const HxfHashMap* restrict map, const void* restrict key) {
    return map->table[map->hash(key)];
}

void* hxfHashMapGetFromHash(const HxfHashMap* restrict map, uint32_t hash) {
    return map->table[hash];
}

void hxfHashMapPut(HxfHashMap* restrict map, const void* restrict key, void* value) {
    map->table[map->hash(key)] = value;
}

void hxfHashMapPutFromHash(HxfHashMap* restrict map, uint32_t hash, void* value) {
    map->table[hash] = value;
}
