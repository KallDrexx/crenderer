#include <malloc.h>
#include <mem.h>
#include <math.h>
#include "list.h"

#define MIN_GROWTH 10
#define RAW_LIST(list) ((int*) list) - 2
#define LIST_LEN(list) (RAW_LIST(list))[0]
#define LIST_CAP(list) (RAW_LIST(list))[1]
#define FIRST_ELEMENT(list) ((int*) list) + 2

void* kcr_list_create(int initialCapacity, size_t itemSize) {
    int* memory = malloc(sizeof(int) * 2 + itemSize * initialCapacity);
    if (memory == NULL) {
        return NULL;
    }

    memory = FIRST_ELEMENT(memory);
    LIST_LEN(memory) = 0;
    LIST_CAP(memory) = initialCapacity;
    return memory;
}

void *kcr_list_append(void *list, void *item, size_t itemSize) {
    if (list == NULL) {
        return NULL;
    }

    int capacity = LIST_CAP(list);
    int count = LIST_LEN(list);
    if (count + 1 > capacity) {
        // Grow it by either 1.25x or by MIN_GROWTH items, whichever is larger.  Item Minimum allows to keep a bunch of
        // small growths reallocating.
        int newCapacity = capacity + (int) roundf((float) capacity * 1.25f);
        if (newCapacity < MIN_GROWTH) {
            newCapacity = capacity + MIN_GROWTH;
        }

        int* newList = realloc(RAW_LIST(list), sizeof(int) * 2 + itemSize * newCapacity);
        if (newList == NULL) {
            return NULL;
        }

        list = FIRST_ELEMENT(newList);
        LIST_CAP(list) = newCapacity;
    }

    void* slot = list + itemSize * count;
    memcpy(slot, item, itemSize);
    LIST_LEN(list) = count + 1;

    return list;
}

void kcr_list_remove(void *list, int index, size_t itemSize) {
    if (list == NULL || index < 0) {
        return;
    }

    int count = LIST_LEN(list);
    if (index >= count) {
        return;
    }

    if (index < count - 1) {
        // Not the last item, so shift everything after it over
        void* slotToRemove = list + itemSize * index;
        int itemsToShift = count - index - 1;
        memmove(slotToRemove, slotToRemove + itemSize, itemsToShift * itemSize);
    }

    LIST_LEN(list) = count - 1;
}

int kcr_list_length(const void *list) {
    if (list == NULL) {
        return 0;
    }

    return LIST_LEN(list);
}

void kcr_list_free(void *list) {
    if (list != NULL) {
        free(RAW_LIST(list));
    }
}
