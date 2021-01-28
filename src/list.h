#ifndef CRENDERER_LIST_H
#define CRENDERER_LIST_H

#include <stddef.h>
#include <stdbool.h>

void* kcr_list_create(size_t itemSize);
bool kcr_list_append(void** list, void* item);
size_t kcr_list_length(const void* list);
void kcr_list_free(void* list);

#endif //CRENDERER_LIST_H
