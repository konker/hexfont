/**
 * libhexfont
 *
 * A library for reading and using fonts encoded in the unifont hex format
 *
 * Copyright 2015, Konrad Markus <konker@luxvelocitas.com>
 *
 * This file is part of libhexfont
 *
 * libhexfont is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libhexfont is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libhexfont.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include "hexfont.h"
#include "hexfont_list.h"

#define HEXFONT_EXAMPLE_TEST_CODEPOINT 0xAD


int main(int argc, char **argv) {
    printf("Hexfont library: %d, %s, %s\n", argc, argv[1], argv[2]);

    char *endptr;
    uint16_t glyph_height = strtol(argv[2], &endptr, 10);
    hexfont * const  example_font = hexfont_load(argv[1], glyph_height);

    printf("Loaded: %d characters\n", example_font->length);

    hexfont_character *c = hexfont_get(example_font, HEXFONT_EXAMPLE_TEST_CODEPOINT);
    printf("Get: %p (%d)\n", c, c->glyph_len);
    size_t y = 0;
    for (y=0; y<c->height; y++) {
        size_t x = 0;
        for (x=0; x<c->width; x++) {
            if (hexfont_get_pixel(c, x, y)) {
                printf("#");
            }
            else {
                printf("-");
            }
        }
        printf("\n");
    }
    printf("Width: %d\n", hexfont_get(example_font, HEXFONT_EXAMPLE_TEST_CODEPOINT)->width);
    printf("Height: %d\n", hexfont_get(example_font, HEXFONT_EXAMPLE_TEST_CODEPOINT)->height);

    // ------------------------------------------------------------------------
    hexfont_list *example_font_list =
                        hexfont_list_create(example_font);
    hexfont * const font = hexfont_list_get_nth(example_font_list, 0);

    printf("hexfont_list: get nth: %d\n", (example_font == font));

    hexfont_list_destroy(example_font_list);
    hexfont_destroy(example_font);
    printf("Goodbye\n");

    return EXIT_SUCCESS;
}
