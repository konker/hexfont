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

#ifndef __HEXFONT_H_
#define __HEXFONT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define HEXFONT_BYTE_WIDTH 8


// An individual character in the font
typedef struct hexfont_character {
    uint32_t codepoint;
    uint8_t *glyph;
    size_t glyph_len;
    uint8_t width;
    uint8_t height;

} hexfont_character;

// Linked list node for keeping glyphs in buckets when there are collisions
typedef struct __hexfont_node {
    uint16_t key;
    hexfont_character * value;
    struct __hexfont_node * next;

} __hexfont_node;

// A list of code points and a way of looking up code points efficiently
typedef struct hexfont {
    __hexfont_node const ** buckets;
    uint16_t length;
    uint16_t glyph_height;

} hexfont;

hexfont * const hexfont_load(const char *file, const uint8_t glyph_height);
hexfont * const hexfont_load_data(const char *data, const uint8_t glyph_height);
void hexfont_destroy(hexfont * const font);
void hexfont_dump_character(hexfont_character * const c, FILE *fp);

const uint16_t __hexfont_hash_function(const uint32_t codepoint, const uint16_t N);

static inline const bool hexfont_character_get_pixel(hexfont_character * const c, const size_t x, const size_t y) {
    // Number of bytes in one row of the glyph
    const size_t glyph_row_bytes = (c->glyph_len / c->height);

    // Find the byte and bit which represent the (x, y) 'pixel coordinate'
    size_t byte = (y * glyph_row_bytes) +
                    (x - (x % HEXFONT_BYTE_WIDTH)) / HEXFONT_BYTE_WIDTH;
    size_t bit = x % HEXFONT_BYTE_WIDTH;

    // Check if the bit is set
    return ((c->glyph[byte] << bit) & 0x80) != 0;
}

static inline hexfont_character * const hexfont_get(hexfont * const font, const uint32_t codepoint) {
    const uint16_t key = __hexfont_hash_function(codepoint, font->length);
    if (font->length < key) {
        return NULL;
    }

    __hexfont_node const * iter = font->buckets[key];
    if (iter == NULL) {
        return NULL;
    }

    while (iter && iter->value && iter->value->codepoint != codepoint) {
        iter = iter->next;
    }

    if (iter == NULL) {
        return NULL;
    }

    return iter->value;
}


#ifdef __cplusplus
}
#endif

#endif // __HEXFONT_H_

