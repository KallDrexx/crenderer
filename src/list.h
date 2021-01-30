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
 * Retrieves the number of items that are currently in the list.
 */
size_t kcr_list_length(const void* list);

/*
 * Will free the memory allocated by the list, including all memory occupied by items contained
 * within the list.  If items contain their own pointers to other data, freeing those must be
 * done prior to freeing the list, otherwise memory leaks will occur.
 */
void kcr_list_free(void* list);

#endif //CRENDERER_LIST_H
