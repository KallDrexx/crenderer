#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mesh.h"
#include "../list.h"

struct KCR_Vec3 cubeVectors[] = {
    {1, 1, -1}, // front top right
    {1, -1, -1}, // front bottom right
    {-1, -1, -1}, // front bottom left
    {-1, 1, -1}, // front top left
    {1, 1, 1}, // back top right
    {1, -1, 1}, // back bottom right
    {-1, -1, 1}, // back bottom left
    {-1, 1, 1}, // back top left
};

struct KCR_Face cubeFaces[] = {
    {2, 3, 0}, // front-top
    {2, 0, 1}, // front-bottom
    {1, 0, 4}, // right-top
    {1, 4, 5}, // right-bottom
    {2, 7, 3}, // left-top
    {2, 6, 7}, // left-bottom
    {2, 1, 5}, // bottom-forward
    {2, 5, 6}, // bottom-rear
    {3, 4, 0}, // top-forward
    {3, 7, 4}, // top-rear
    {5, 4, 7}, // back-top
    {5, 7, 6}, // back-bottom
};

struct KCR_Mesh* kcr_mesh_from_obj_file(char* filename) {
    #define BUFFER_SIZE 1000

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to load file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }

    struct KCR_Vec3* vertexList = kcr_list_create(10, sizeof(struct KCR_Vec3));
    struct KCR_Face* faceList = kcr_list_create(10, sizeof(struct KCR_Face));
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
                vertexList = kcr_list_append(vertexList, &vertex, sizeof(struct KCR_Vec3));
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

                    if (indices[index] <= 0 || indices[index] > kcr_list_length(vertexList)) {
                        fprintf(stderr, "Line %i has invalid face index of %i\n", line, indices[index]);
                        goto nextLoop;
                    }

                    if (readCount >= 3) {
                        int v1 = indices[firstIndex % 3];
                        int v2 = indices[(firstIndex + 1) % 3];
                        int v3 = indices[(firstIndex + 2) % 3];

                        // obj has them listed in CCW, right now our engine uses cw
                        struct KCR_Face face = {v3 - 1, v2 - 1, v1 - 1};
                        faceList = kcr_list_append(faceList, &face, sizeof(struct KCR_Face));
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

    struct KCR_Vec3 rotation = {0,0,0};
    struct KCR_Mesh* mesh = malloc(sizeof(struct KCR_Mesh));
    mesh->vertexList = vertexList;
    mesh->faceList = faceList;
    mesh->rotation = rotation;

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
