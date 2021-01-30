#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "list.h"

#define MIN_GROWTH 10

struct List {
    size_t length;
    size_t capacity;
    size_t itemSize;
    char data[];
};

void* kcr_list_create(size_t itemSize) {
    struct List* list = malloc(sizeof(struct List) + itemSize);
    list->length = 0;
    list->capacity = 0;
    list->itemSize = itemSize;

    return list->data;
}

void* kcr_list_new_item(void **list) {
    assert(list != NULL);
    assert(*list != NULL);

    // Since *list points to the address of the first element, we have to look at the bytes prior to that
    // address to get the underlying list structure
    struct List* realList = (struct List*) ((*list) - sizeof(struct List));

    size_t capacity = realList->capacity;
    size_t count = realList->length;
    if (count + 1 > capacity) {
        size_t newCapacity = capacity + (capacity * 5) / 4;
        if (newCapacity < MIN_GROWTH) {
            newCapacity = capacity + MIN_GROWTH;
        }

        struct List* newList = realloc(realList, sizeof(struct List) + realList->itemSize * newCapacity);
        if (newList == NULL) {
            return NULL;
        }

        realList = newList;
        realList->capacity = newCapacity;

        // Since realloc may have moved the data to accommodate the new size, make sure the consumer's pointer
        // is updated.
        *list = (char*) realList->data;
    }

    char* slot = (char*) realList + (sizeof(struct List) + realList->itemSize * count);
    memset(slot, 0, realList->itemSize);
    realList->length++;

    return slot;
}

size_t kcr_list_length(const void *list) {
    if (list == NULL) {
        return 0;
    }

    struct List* realList = (struct List*) ((char*)(list) - sizeof(struct List));
    return realList->length;
}

void kcr_list_free(void* list) {
    if (list != NULL) {
        struct List* realList = (struct List*) ((char*)(list) - sizeof(struct List));
        free(realList);
    }
}
