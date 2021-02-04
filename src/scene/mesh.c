#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mesh.h"
#include "../list.h"
#include "../gfx/color_library.h"

#define COLOR_COUNT 41

uint32_t colors[] = {RED, MAROON, BROWN, OLIVE, ORANGE, GOLD, GREEN, TEAL, TURQUOISE, BLUE, YELLOW, NAVAJO_WHITE, PURPLE,
                     PINK, WHEAT, CORN_SILK, TAN, AZURE, SLATE_GRAY, WHITE, LAVENDER, STEEL_BLUE, AQUA, GREEN_YELLOW,
                     DARK_RED, LIGHT_SALMON, FIREBRICK, KHAKI, LAWN_GREEN, SEA_GREEN, CYAN, SKY_BLUE, INDIGO, DARK_ORCHID,
                     MAGENTA, CORN_FLOWER_BLUE, LIGHT_CORAL, HOT_PINK, CHOCOLATE, MEDIUM_AQUA_MARINE, BROWN};

bool kcr_mesh_from_obj_file(struct KCR_Mesh* mesh, char* filename) {
    #define BUFFER_SIZE 1000

    assert(mesh != NULL);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to load file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    struct KCR_Vec3* vertexList = kcr_list_create(sizeof(struct KCR_Vec3));
    struct KCR_Face* faceList = kcr_list_create(sizeof(struct KCR_Face));
    char buffer[BUFFER_SIZE];
    unsigned int line = 0;

    while(fgets(buffer, BUFFER_SIZE, file) != NULL) {
        // Assume no line is more than 1000 characters, so every iteration is a new line
        if (strlen(buffer) >= 3) {
            if (strncmp(buffer, "v ", 2) == 0) {
                // vertex definition
                char* start = buffer + 1;
                char* end;
                float values[3];
                for (int x = 0; x < 3; x++) {
                    values[x] = strtof(start, &end);
                    if (start == end) {
                        fprintf(stderr, "Line %i has vertex data in an unknown format.\n", line);
                        goto nextLoop;
                    }

                    start = end;
                }

                struct KCR_Vec3* vertex = kcr_list_new_item((void**) &vertexList);
                vertex->x = values[0];
                vertex->y = values[1];
                vertex->z = values[2];\
            }

            else if (strncmp(buffer, "f ", 2) == 0) {
                // face definition
                char* start = buffer + 1;
                char* end;
                int readCount = 0;
                int* indices = kcr_list_create(sizeof(int));
                while(1) {
                    int value = strtol(start, &end, 10);
                    if (start == end) {
                        size_t length = kcr_list_length(indices);
                        if (length >= 3) {
                            for (size_t i = 0; i < length - 1; i += 2) {
                                int v1 = indices[i % length];
                                int v2 = indices[(i + 1) % length];
                                int v3 = indices[(i + 2) % length];

                                struct KCR_Face* face = kcr_list_new_item((void**) &faceList);

                                // OBJ files lists them listed in CCW, but right now our engine uses cw.  OBJ files also
                                // use 1 based indexing, but we obviously use zero based indexing
                                face->vertexIndex1 = v3 - 1;
                                face->vertexIndex2 = v2 - 1;
                                face->vertexIndex3 = v1 - 1;
                                face->color = colors[(line + i) % COLOR_COUNT];
                            }
                        } else {
                            fprintf(stderr, "Line %i has less than 3 faces\n", line);
                        }

                        break;
                    }

                    if (value <= 0 || value > (int) kcr_list_length(vertexList)) {
                        fprintf(stderr, "Line %i has invalid face index of %i\n", line, indices[readCount]);
                        break;
                    }

                    kcr_list_new_item((void**) &indices);
                    indices[readCount] = value;

                    start = end;
                    while (*start == '/') {
                        strtof(start + 1, &end);
                        start = end;
                    }
                    readCount++;
                }
                kcr_list_free(indices);
            }
        }

        nextLoop:
            line++;
    }

    mesh->vertexList = vertexList;
    mesh->faceList = faceList;

    fclose(file);

    return true;
}

void kcr_mesh_uninit(struct KCR_Mesh *mesh) {
    if (mesh != NULL) {
        kcr_list_free(mesh->faceList);
        kcr_list_free(mesh->vertexList);

        mesh->faceList = NULL;
        mesh->vertexList = NULL;
    }
}

bool kcr_mesh_instance_init(struct KCR_MeshInstance* instance, struct KCR_Mesh *mesh) {
    assert(instance != NULL);
    assert(mesh != NULL);

    instance->mesh = mesh;
    instance->position = (struct KCR_Vec3) {0};
    instance->rotation = (struct KCR_Vec3) {0};

    return true;
}

__attribute__((unused)) void kcr_mesh_instance_uninit(__attribute__((unused)) struct KCR_MeshInstance* instance) {
}
