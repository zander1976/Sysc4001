#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef __RENDER_H__
#define __RENDER_H__

typedef enum {
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE
} color_t;

typedef struct {
    int row;
    int col;
    char *pixels;
    color_t *color; 
} surface_t;

surface_t* _render_create_surface(int col, int row);
void _render_clear_surface(surface_t* self);
void _render_delete_surface(surface_t* self);
void _render_write_string(surface_t* self, int x, int y, char *output, color_t color);
void _render_write_int(surface_t* self, int x, int y, int number, color_t color);
void _render_display_frame(surface_t* self);

#endif //__RENDER_H__

#ifdef __RENDER_IMPLEMENTATION__

surface_t* _render_create_surface(int col, int row) {

    surface_t* surface = malloc(sizeof(surface_t));
    assert(surface != NULL);

    surface->col = col;
    surface->row = row;

    surface->pixels = malloc((row * col + 1) * sizeof(char));
    assert(surface->pixels != NULL);
    surface->pixels[row * col] = '\0';

    surface->color = malloc((row * col + 1) * sizeof(color_t));
    assert(surface->color != NULL);
    surface->color[row * col] = '\0';

    _render_clear_surface(surface);
    return surface;
}

void _render_clear_surface(surface_t* self){ 
    for(int i = 0; i < self->row; i++) {
        for(int j = 0; j < self->col; j++) {
            self->pixels[i * self->col + j] = ' ';
            self->color[i * self->col + j] = COLOR_RED;
        }
    }
}

void _render_delete_surface(surface_t* self) {
    free(self->pixels);
    free(self->color);
    free(self);
}

void _render_write_string(surface_t* self, int x, int y, char *output, color_t color) {
    if (x < 0 || x >= self->col || y < 0 || y >= self->row) {
        return;
    }
    int location = y * self->col + x;
    int len = strlen(output);
    for(int i = 0; i < len; i++) {
        if ((x + i) >= self->col) {
            break;
        }
        self->pixels[location + i] = output[i];
        self->color[location + i] = color;
    }
}


void _render_write_int(surface_t* self, int x, int y, int number, color_t color) {
    if (x < 0 || x >= self->col || y < 0 || y >= self->row) {
        return;
    }
    char output[20];
    sprintf(output, "%d", number);
    int location = y * self->col + x;
    int len = strlen(output);
    for (int i = 0; i < len; i++) {
        if ((x + i) >= self->col) {
            break;
        }
        self->pixels[location + i] = output[i];
        self->color[location + i] = color;
    }
}


void _render_display_frame(surface_t* self) {
    system("clear");
    printf("\033[?25l");  // Disable cursor
    printf("\033[?7l");   // Disable bell
    for (int i = 0; i < self->row; i++) {
        for (int j = 0; j < self->col; j++) {
            //printf("\x1b[3%dm%c" COLOR_RESET, self->color[i * self->col + j], self->pixels[i * self->col + j]);
            printf("\x1b[3%dm%c\x1b[0m", (int)self->color[i * self->col + j], self->pixels[i * self->col + j]);

        }
        printf("\n");
    }
    printf("\033[?7h");   // Enable bell
    printf("\033[?25h");  // Enable cursor
}


#endif //__RENDER_IMPLEMENTATION__