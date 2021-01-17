#include <stdbool.h>
#include "display.h"
#include "input.h"
#include "vector.h"
#include "draw.h"

bool isRunning;
InputState inputState;

#define POINT_COUNT 9 * 9 * 9
const float FOV_FACTOR = 120;

Vector3 cubePoints[POINT_COUNT];
Vector2 projectedPoints[POINT_COUNT];

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
        Vector2 projected_point = project_to_2d(&point);

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

        draw_rect(x - 4, y - 4, 8, 8, 0xFFFF0000);
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
