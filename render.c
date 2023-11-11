#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define __RENDER_IMPLEMENTATION__
#include "render.h"

int main() {

    surface_t* surface = _render_create_surface(10, 5);
    assert(surface != NULL);

    char* message = "Hello World!";
    //for(int i = 0; i < 10; i++) {
    //    _write_int(surface, 0, i, i, i);
    //    _write_string(surface, 3, i, &message[i], i);
    //    _display_frame(surface);
    //}
    _render_clear_surface(surface);

    _render_write_int(surface, 0, 0, 101, COLOR_CYAN);
    _render_write_int(surface, 0, 1, 102, COLOR_CYAN);
    _render_write_int(surface, 0, 2, 103, COLOR_CYAN);
    _render_write_int(surface, 8, 0, 201, COLOR_GREEN);
    _render_write_int(surface, 8, 1, 202, COLOR_GREEN);
    _render_write_int(surface, 8, 2, 203, COLOR_GREEN);

    //_write_string(surface, 5, 0, "Hello World", COLOR_BLUE);
    _render_write_string(surface, 5, 1, "Hello World", COLOR_CYAN);
    _render_write_string(surface, 5, 2, "ASDFASDF", COLOR_CYAN);
    _render_write_string(surface, 5, 3, "DGHDFGHDFG", COLOR_CYAN);

    _render_display_frame(surface);
    _render_delete_surface(surface);

    return 0;
}
