#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mesh.h"
#include "../list.h"

struct KCR_Mesh* kcr_mesh_from_obj_file(char* filename) {
    #define BUFFER_SIZE 1000

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to load file '%s': %s\n", filename, strerror(errno));
        return NULL;
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

                struct KCR_Vec3 vertex = {values[0], values[1], values[2]};
                kcr_list_append((void**) &vertexList, &vertex);
            }

            else if (strncmp(buffer, "f ", 2) == 0) {
                // face definition
                char* start = buffer + 1;
                char* end;
                int firstIndex = 0;
                int readCount = 0;
                int indices[3];
                while(1) {
                    int index = readCount % 3;
                    indices[index] = strtol(start, &end, 10);
                    readCount++;
                    if (start == end) {
                        break;
                    }

                    if (indices[index] <= 0 || indices[index] > (int) kcr_list_length(vertexList)) {
                        fprintf(stderr, "Line %i has invalid face index of %i\n", line, indices[index]);
                        goto nextLoop;
                    }

                    if (readCount >= 3) {
                        int v1 = indices[firstIndex % 3];
                        int v2 = indices[(firstIndex + 1) % 3];
                        int v3 = indices[(firstIndex + 2) % 3];

                        // obj has them listed in CCW, right now our engine uses cw
                        struct KCR_Face face = {v3 - 1, v2 - 1, v1 - 1};
                        kcr_list_append((void**) &faceList, &face);
                        firstIndex++;
                    }

                    start = end;
                    while (*start == '/') {
                        strtof(start + 1, &end);
                        start = end;
                    }
                }
            }
        }

        nextLoop:
            line++;
    }

    struct KCR_Mesh* mesh = malloc(sizeof(struct KCR_Mesh));
    mesh->vertexList = vertexList;
    mesh->faceList = faceList;

    fclose(file);

    return mesh;
}

void kcr_mesh_free(struct KCR_Mesh *mesh) {
    if (mesh != NULL) {
        kcr_list_free(mesh->faceList);
        kcr_list_free(mesh->vertexList);
    }

    free(mesh);
}

struct KCR_MeshInstance* kcr_mesh_instance_create(struct KCR_Mesh *mesh) {
    assert(mesh != NULL);

    struct KCR_MeshInstance* instance = calloc(1, sizeof(struct KCR_MeshInstance));
    instance->mesh = mesh;

    return instance;
}

void kcr_mesh_instance_free(struct KCR_MeshInstance* instance) {
    free(instance);
}
