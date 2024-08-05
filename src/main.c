#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define WIDTH 800
#define HEIGHT 600
#define CELL_SIZE 1
#define ROWS (HEIGHT / CELL_SIZE)
#define COLS (WIDTH / CELL_SIZE)
#define TOTAL_CELLS (ROWS * COLS)


Vector2 index_to_vec(size_t i) {
    int x = (i % COLS) * CELL_SIZE;
    int y = (i / COLS) * CELL_SIZE;
    return (Vector2){
        .x = x,
        .y = y,
    };
}

size_t vec_to_index(int x, int y) {
    return y * COLS + x;
}

void game_of_life(bool* cells, bool* temp_cells, size_t cells_len) {
    /*
     * 1. Any live cell with less than two live neighbours dies, as if by
     * underpopulation.
     * 2. Any live cell with two or three live neighbours lives on to the next
     * generation.
     * 3. Any live cell with more than three live neighbours dies, as if by
     * overpopulation.
     * 4. Any dead cell with exactly three live neighbours becomes a live cell, as
     * if by reproduction.
     * */


    // relative to current cell
    Vector2 neighbours[8] = {
        {.x = -1, .y = -1}, {.x = 0, .y = -1}, {.x = 1, .y = -1},
        {.x = -1, .y = 0},  {.x = 1, .y = 0},  {.x = -1, .y = 1},
        {.x = 0, .y = 1},   {.x = 1, .y = 1},
    };

    for (size_t cell = 0; cell < cells_len; cell++) {
        Vector2 pos = index_to_vec(cell);
        int live_cells = 0;

        for (int n = 0; n < 8; n++) {
            Vector2 standard = (Vector2) {
                .x = pos.x / CELL_SIZE,
                .y = pos.y / CELL_SIZE,
            };
            Vector2 neighbour = Vector2Add(standard, neighbours[n]);
            size_t index = vec_to_index(neighbour.x, neighbour.y);

            if (
                neighbour.x < 0 || neighbour.x * CELL_SIZE >= WIDTH || 
                neighbour.y < 0 || neighbour.y * CELL_SIZE >= HEIGHT
               ) continue;

            if (cells[index])
                live_cells++;
        }

        if (live_cells < 2 || live_cells > 3) {
            temp_cells[cell] = false;
        } else if (!cells[cell] && live_cells == 3) {
            temp_cells[cell] = true;
        } else {
            temp_cells[cell] = cells[cell];
        }
    }

    memcpy(cells, temp_cells, cells_len);
}

void randomize(bool* cells, size_t cell_len) {
    for (int i = 0; i < cell_len; i++) {
        bool value = GetRandomValue(0, 1);
        cells[i] = value;
    }
}

int main() {
    bool* cells = malloc(TOTAL_CELLS);
    bool* temp_cells = malloc(TOTAL_CELLS);
    bool start = false;

    InitWindow(WIDTH, HEIGHT, "GOL");

    while (!WindowShouldClose()) {
        ClearBackground(BLACK);
        SetTargetFPS(20);

        Vector2 mouse_pos = GetMousePosition();

        Vector2 mouse_cell_pos = {
            .x = (float)((int)mouse_pos.x / CELL_SIZE),
            .y = (float)((int)mouse_pos.y / CELL_SIZE),
        };

        size_t cell_index = mouse_cell_pos.y * COLS + mouse_cell_pos.x;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            cells[cell_index] = !cells[cell_index];
        }

        switch (GetKeyPressed()) {
            case KEY_SPACE:
                start = !start;
                break;
            case KEY_R:
                randomize(cells, TOTAL_CELLS);
                break;
            case KEY_C:
                memset(cells, 0, TOTAL_CELLS);
                memset(temp_cells, 0, TOTAL_CELLS);
                start = false;
                break;
        }

        BeginDrawing();

        for (int i = 0; i < TOTAL_CELLS; i++) {
            if (cells[i]) {
                Vector2 pos = index_to_vec(i);
                DrawRectangle(
                    pos.x, pos.y,
                    CELL_SIZE, CELL_SIZE,
                    WHITE
                );
            }
        }

        if (start) {
            game_of_life(cells, temp_cells, TOTAL_CELLS);
        }

        DrawRectangle(
            CELL_SIZE * mouse_cell_pos.x,
            CELL_SIZE * mouse_cell_pos.y,
            CELL_SIZE, CELL_SIZE,
            GetColor(0xAEAEAE55)
        );
        EndDrawing();
    }
    CloseWindow();
    free(cells);
    free(temp_cells);
    return 0;
}
