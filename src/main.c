#include <stdbool.h>
#include "display.h"
#include "input.h"
#include "vector.h"
#include "draw.h"

bool isRunning;
InputState inputState;

#define POINT_COUNT 9 * 9 * 9
const float FOV_FACTOR = 800;

Vector3 cubePoints[POINT_COUNT];
Vector2 projectedPoints[POINT_COUNT];
Vector3 eye = {0, 0, -5};

bool setup(void) {
    isRunning = initialize_display();

    int index = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                Vector3 vector = { .x = x, .y = y, .z = z};
                cubePoints[index] = vector;

                index++;
            }
        }
    }

    return true;
}

void process_input(void) {
    process_events(&inputState);

    if (inputState.quit_requested) {
        isRunning = false;
    }
}

void update(void) {
    for (int i = 0; i < POINT_COUNT; i++) {
        Vector3 point = cubePoints[i];
        Vector2 projected_point = vector3_project(&point, &eye);

        projectedPoints[i] = projected_point;
    }
}

void render(void) {
    begin_render_cycle();

    int horizontalCenter = windowWidth / 2;
    int verticalCenter = windowHeight / 2;

    for (int i = 0; i < POINT_COUNT; i++) {
        Vector2 point = projectedPoints[i];
        float x = point.x * FOV_FACTOR + horizontalCenter;
        float y = point.y * FOV_FACTOR + verticalCenter;

        float mult = (1 - cubePoints[i].z) / 2.0f;
        uint8_t red = (uint32_t) (((float) 0xFF) * mult);
        uint32_t color = (((uint32_t) red) << 4 * 4) | 0xFF000000;

        draw_rect(x - 2, y - 2, 4, 4, color);
    }

    finish_render_cycle();
}

int main(int argc, char* args[]) {
    setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }

    free_display_resources();

    return 0;
}
