#ifndef CRENDERER_LIST_H
#define CRENDERER_LIST_H

#include <stddef.h>
#include <stdbool.h>

/*
 * Create a new growable packed collection of items and returns a pointer to the first
 * address of the list.  The collection will be empty at this time and must not be
 * accessed until items are added.
 */
void* kcr_list_create(size_t itemSize);

/*
 * Adds a new item to the collection and returns a pointer to that item's position.  Each
 * item created will be zeroed out.
 */
void* kcr_list_new_item(void** list);

/*
 * Adds multiple new items to the collection and returns a pointer to the first new item's position.
 * Each item created will be zeroed out.
 */
void* kcr_list_new_items(void** list, size_t countToAdd);

/*
 * Retrieves the number of items that are currently in the list.
 */
size_t kcr_list_length(const void* list);

/*
 * Sets the length of the list to zero while keeping its capacity the same.  Does not zero
 * out memory.
 */
void kcr_list_clear(void* list);

/*
 * Will free the memory allocated by the list, including all memory occupied by items contained
 * within the list.  If items contain their own pointers to other m, freeing those must be
 * done prior to freeing the list, otherwise memory leaks will occur.
 */
void kcr_list_free(void* list);

#endif //CRENDERER_LIST_H
