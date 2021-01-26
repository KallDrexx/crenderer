#ifndef CRENDERER_LIST_H
#define CRENDERER_LIST_H

#include <stddef.h>

/**
 * Creates a new growable list
 * @param initialCapacity how many items the list should initially have memory for
 * @param itemSize The size of each item
 * @return returns a pointer to the newly created list, or `NULL` if list creation failed
 */
void* kcr_list_create(int initialCapacity, size_t itemSize);

/**
 * Appends a new item to the end of an existing list.  If the list does not have the capacity for the item than
 * the list will be resized to account for it.
 * @param list Existing list to append the item onto
 * @param item A pointer to the item to add to the list
 * @param itemSize The memory size of the item to add.  This must be the same as specified during creation
 * @return returns a pointer to the latest location of the list.  Consumers must always reference the list from
 * the value returned and not re-use the previous pointer, as when the list grows it may end up in a new spot
 * to accomodate the size it needed to grow into.
 *
 * Will return `NULL` if appending failed
 */
void* kcr_list_append(void* list, void* item, size_t itemSize);

/**
 * Removes the item from the list at the specified index
 * @param list list to remove the item from
 * @param index Index of the item to remove
 * @param itemSize The memory size of the items in the list
 */
void kcr_list_remove(void* list, int index, size_t itemSize);

/**
 * Gets the number of items that exist in the list
 * @param list The existing list
 * @return Number of items in the list
 */
int kcr_list_length(const void* list);

/**
 * Frees the list
 */
void kcr_list_free(void* list);

#endif //CRENDERER_LIST_H
