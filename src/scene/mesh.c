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
    struct KCR_Vec3* normalList = kcr_list_create(sizeof(struct KCR_Vec3));
    struct KCR_Vec2* textureList = kcr_list_create(sizeof(struct KCR_Vec2));

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
                        fprintf(stderr, "Line %i has a vertex in an unknown format.\n", line);
                        goto nextLoop;
                    }

                    start = end;
                }

                struct KCR_Vec3* vertex = kcr_list_new_item((void**) &vertexList);
                vertex->x = values[0];
                vertex->y = values[1];
                vertex->z = values[2];
            }

            else if (strncmp(buffer, "vn", 2) == 0) {
                // normal definition
                char* start = buffer + 2;
                char* end;
                float values[3];
                for (int x = 0; x < 3; x++) {
                    values[x] = strtof(start, &end);
                    if (start == end) {
                        fprintf(stderr, "Line %i has a normal in an unknown format.\n", line);
                        goto nextLoop;
                    }

                    start = end;
                }

                // obj vertex normals are not guaranteed to be unit vectors, so lets precompute that ahead of time.
                // Not sure non-unit normals are useful for us.
                struct KCR_Vec3 temp = (struct KCR_Vec3) {values[0], values[1], values[2]};
                temp = kcr_vec3_normalize(&temp);

                struct KCR_Vec3* normal = kcr_list_new_item((void**) &normalList);

                // OBJ files are CCW, but we are CW and left handed.  So we need to invert the normal vectors to align
                // with our coordinate system
                normal->x = -temp.x;
                normal->y = -temp.y;
                normal->z = -temp.z;
            }

            else if (strncmp(buffer, "vt", 2) == 0) {
                // Texture coordinates
                char* start = buffer + 2;
                char* end;
                float values[2];
                for (int x = 0; x < 2; x++) {
                    values[x] = strtof(start, &end);
                    if (start == end) {
                        fprintf(stderr, "Line %i has texture coordinates in an unknown format.\n", line);
                        goto nextLoop;
                    }

                    start = end;
                }

                struct KCR_Vec2* texture = kcr_list_new_item((void**) &textureList);
                texture->x = values[0];
                texture->y = values[1];
            }

            else if (strncmp(buffer, "f ", 2) == 0) {
                // face definition
                char* start = buffer + 1;
                char* end;
                int readCount = 0;
                struct KCR_Vertex* vertices = kcr_list_create(sizeof(struct KCR_Vertex));
                while(1) {
                    int value = strtol(start, &end, 10);
                    if (start == end) {
                        size_t length = kcr_list_length(vertices);
                        if (length >= 3) {
                            for (size_t i = 0; i < length - 1; i += 2) { // +2 to properly deal with quads
                                struct KCR_Vertex v1 = vertices[i % length];
                                struct KCR_Vertex v2 = vertices[(i + 1) % length];
                                struct KCR_Vertex v3 = vertices[(i + 2) % length];

                                struct KCR_Face* face = kcr_list_new_item((void**) &faceList);

                                // OBJ files lists them listed in CCW, but right now our engine uses cw.  OBJ files also
                                // use 1 based indexing, but we obviously use zero based indexing
                                face->v1 = v3;
                                face->v2 = v2;
                                face->v3 = v1;
                                face->color = colors[(line + i) % COLOR_COUNT];
                            }
                        } else {
                            fprintf(stderr, "Line %i has less than 3 faces\n", line);
                        }

                        break;
                    }

                    if (value <= 0 || value > (int) kcr_list_length(vertexList)) {
                        fprintf(stderr, "Line %i has an invalid face vertex index of %i\n", line, value);
                        break;
                    }

                    struct KCR_Vertex* vertexDetails = kcr_list_new_item((void**) &vertices);
                    vertexDetails->positionIndex = value - 1; // change from 1 to 0 based indexes
                    vertexDetails->normalIndex = -1;
                    vertexDetails->textureIndex = -1;

                    start = end;
                    if (*start == '/') {
                        // vertex texture
                        value = strtol(start + 1, &end, 10);
                        if (value != 0) {
                            if (value > (int) kcr_list_length(textureList)) {
                                fprintf(stderr, "Line %i has an invalid face texture index of %i\n", line, value);
                                break;
                            }

                            vertexDetails->textureIndex = value - 1; // change from 1 to 0 based indexes
                        }

                        start = end;
                        if (*start == '/') {
                            // vertex normal
                            value = strtol(start + 1, &end, 10);
                            if (value != 0) {
                                if (value > (int) kcr_list_length(normalList)) {
                                    fprintf(stderr, "Line %i has an invalid face normal index of %i\n", line, value);
                                    break;
                                }

                                vertexDetails->normalIndex = value - 1; // change from 1 to 0 based indexes
                            }

                            start = end;
                        }
                    }

                    readCount++;
                }

                kcr_list_free(vertices);
            }
        }

        nextLoop:
            line++;
    }

    mesh->vertexList = vertexList;
    mesh->faceList = faceList;
    mesh->textureCoordsList = textureList;
    mesh->normalList = normalList;
    mesh->texture = kcr_texture_red_brick();

    fclose(file);

    return true;
}

void kcr_mesh_uninit(struct KCR_Mesh *mesh) {
    if (mesh != NULL) {
        kcr_list_free(mesh->faceList);
        kcr_list_free(mesh->vertexList);
        kcr_list_free(mesh->textureCoordsList);
        kcr_list_free(mesh->normalList);

        mesh->faceList = NULL;
        mesh->vertexList = NULL;
        mesh->textureCoordsList = NULL;
        mesh->normalList = NULL;

        kcr_texture_free(mesh->texture);
    }
}

bool kcr_mesh_instance_init(struct KCR_MeshInstance* instance, struct KCR_Mesh *mesh) {
    assert(instance != NULL);
    assert(mesh != NULL);

    instance->mesh = mesh;
    instance->position = (struct KCR_Vec3){0};
    instance->rotation = (struct KCR_Vec3){0};
    instance->scale = (struct KCR_Vec3) {1.0f, 1.0f, 1.0f};

    return true;
}

void kcr_mesh_instance_uninit(struct KCR_MeshInstance* instance) {
    instance->mesh = NULL;
}
