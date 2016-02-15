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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hexfont.h"

#define HEXFONT_BYTE_WIDTH 8

// Enough for 1234:X...
#define HEXFONT_MIN_DATA_ITEM_LEN 6

// Codepoints and glyphs are hex strings
#define HEXFONT_CODEPOINT_NUMBER_BASE 16
#define HEXFONT_GLYPH_NUMBER_BASE 16

// Index of ':' character
#define HEXFONT_DATA_ITEM_SEP_POSITION 4

// Default width for non-printable characters
#define HEXFONT_DEFAULT_NON_PRINTABLE_WIDTH 3


static const uint16_t __hexfont_hash_function(const uint16_t codepoint, const uint16_t N);
static hexfont * const __hexfont_load_exec(FILE *fp, const uint8_t glyph_height);
static void __hexfont_parse_glyph(uint8_t **glyph, size_t *glyph_len, char * const glyph_chars, const size_t glyph_chars_len);
static const uint16_t __hexfont_calculate_width(uint8_t * const glyph, const size_t glyph_len, const uint16_t glyph_height);
static void __hexfont_add_character(hexfont * const font, uint16_t codepoint, uint8_t * const glyph, const size_t glyph_len, const uint16_t glyph_height);


hexfont * const hexfont_load(const char *file, const uint8_t glyph_height) {
    // Read in the file
    FILE *fp;

    fp = fopen(file, "r");
    if (fp == NULL) {
        return NULL;
    }

    return __hexfont_load_exec(fp, glyph_height);
}

hexfont * const hexfont_load_data(const char *data, const uint8_t glyph_height) {
    FILE *fp;

    // Treat the data as a file
    fp = fmemopen((char *)data, strlen(data), "r");
    if (fp == NULL) {
        return NULL;
    }

    return __hexfont_load_exec(fp, glyph_height);
}


hexfont_character * const hexfont_get(hexfont * const font, const uint16_t codepoint) {
    const uint16_t key = __hexfont_hash_function(codepoint, font->length);
    if (font->length < key) {
        return NULL;
    }

    __hexfont_node const * iter = font->buckets[key];
    if (iter == NULL) {
        return NULL;
    }

    while (iter->key != key) {
        iter = iter->next;
    }
    return iter->value;
}

const bool hexfont_character_get_pixel(hexfont_character * const c, const size_t x, const size_t y) {
    // Number of bytes in one row of the glyph
    const size_t glyph_row_bytes = (c->glyph_len / c->height);

    // Find the byte and bit which represent the (x, y) 'pixel coordinate'
    size_t byte = (y * glyph_row_bytes) +
                    (x - (x % HEXFONT_BYTE_WIDTH)) / HEXFONT_BYTE_WIDTH;
    size_t bit = x % HEXFONT_BYTE_WIDTH;

    // Check if the bit is set
    return ((c->glyph[byte] << bit) & 0x80) != 0;
}

void hexfont_destroy(hexfont * const font) {
    uint16_t i = 0;
    for (i=0; i<font->length; i++) {
        __hexfont_node const * tmp;
        __hexfont_node const * iter = font->buckets[i];

        while (iter) {
            tmp = iter;
            iter = iter->next;

            free(tmp->value->glyph);
            free((hexfont_character *)tmp->value);
            free((__hexfont_node *)tmp);
        }
    }
    free(font->buckets);
    free(font);
}

void hexfont_dump_character(hexfont_character * const c, FILE *fp) {
    int16_t by, bx;
    for (by=0; by<c->height; by++) {
        for (bx=0; bx<c->width; bx++) {
            if (hexfont_character_get_pixel(c, bx, by)) {
                fprintf(fp, "# ");
            }
            else {
                fprintf(fp, ". ");
            }
        }
        fprintf(fp, "\n");
    }
}

// ----------------------------------------------------------------------------
// Static helpers
static const uint16_t __hexfont_hash_function(const uint16_t codepoint, const uint16_t N) {
    return (codepoint % N);
}

static hexfont * const __hexfont_load_exec(FILE *fp, const uint8_t glyph_height) {
    char *line = NULL;
    char *endptr = NULL;
    size_t len = 0;
    size_t read;

    // Count the number of codepoints
    uint16_t N = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line == NULL || read < HEXFONT_MIN_DATA_ITEM_LEN) {
            continue;
        }
        N++;
    }
    free(line);

    // Allocate memory for the hexfont structure
    hexfont * const font = malloc(sizeof(hexfont));

    // Allocate memory for the buckets
    font->length = N;
    font->glyph_height = glyph_height;
    font->buckets = calloc(N, sizeof(__hexfont_node));

    // Second pass over the data
    line = NULL;
    len = 0;
    fseek(fp, 0, SEEK_SET);
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line == NULL || read < HEXFONT_MIN_DATA_ITEM_LEN) {
            continue;
        }

        // Parse the codepoint number
        const uint16_t codepoint =
            (const uint16_t)strtol(line, &endptr, HEXFONT_CODEPOINT_NUMBER_BASE);

        // Extract the glyph chars into an array of bytes
        size_t glyph_len;
        uint8_t *glyph;
        __hexfont_parse_glyph(
                            &glyph,
                            &glyph_len,
                            endptr + 1,
                            strlen(endptr + 1) - 1);

        // Create a hexfont_character
        __hexfont_add_character(font, codepoint, glyph, glyph_len, glyph_height);
    }
    free(line);

    // Tidy up file pointer
    fclose(fp);

    return font;
}

/**
 * Convert a character string of hex-digit pairs into an array of bytes
*/
static void __hexfont_parse_glyph(uint8_t **glyph, size_t *glyph_len, char * const glyph_chars, const size_t glyph_chars_len) {
    // Calculate the number of hex pairs in the glyph_chars string
    *glyph_len = glyph_chars_len / 2;

    // Allocate that many uint8_t items in the glyph array
    *glyph = calloc(*glyph_len, sizeof(**glyph));

    // Parse each hex pair to an unsigned int
    size_t i = 0;
    for (i=0; i<*glyph_len; i++) {
        sscanf(glyph_chars + 2*i, "%02hhx", (*glyph)+i);
    }
}

static const uint16_t __hexfont_calculate_width(uint8_t * const glyph, const size_t glyph_len, const uint16_t glyph_height) {
    // Character width of the glyph, usually 1 or 2
    const size_t glyph_char_width = (glyph_len / glyph_height);

    // Each byte encodes 8 bits, or 8 'pixels'
    uint16_t last_on = 0;

    // Loop through each byte in the glyph
    size_t i = 0;
    for (i=0; i<glyph_len; i++) {
        // Loop through each bit in the byte
        size_t j = 0;
        for (j=0; j<HEXFONT_BYTE_WIDTH; j++) {
            // Find the 'x coordinate' represented by the jth bit in the ith byte
            size_t x = ((i % glyph_char_width) * HEXFONT_BYTE_WIDTH) + j;

            // Check if the bit is set
            if ((glyph[i] << j) & 0x80) {
                // Check if this bit is more 'right' than previously seen bits
                if (x > last_on) {
                    last_on = x;
                }
            }
        }
    }

    // Convert an index to a width
    last_on += 1;

    // Adjust, mostly for SPACE character
    if (last_on == 1) {
        last_on = HEXFONT_DEFAULT_NON_PRINTABLE_WIDTH;
    }

    return last_on;
}

static void __hexfont_add_character(hexfont * const font, uint16_t codepoint, uint8_t * const glyph, const size_t glyph_len, const uint16_t glyph_height) {
    hexfont_character * const character = malloc(sizeof(hexfont_character));
    __hexfont_node * const node = malloc(sizeof(__hexfont_node));

    // Initialize a character
    character->codepoint = codepoint;
    character->glyph = glyph;
    character->glyph_len = glyph_len;
    character->height = glyph_height;
    character->width = __hexfont_calculate_width(glyph, glyph_len, glyph_height);

    // Initialize a node to hold the character
    node->key = __hexfont_hash_function(codepoint, font->length);
    node->value = character;
    node->next = NULL;

    // Add the glyph to the list
    if (font->buckets[node->key] == NULL) {
        // First item in the bucket
        font->buckets[node->key] = node;
    }
    else {
        // Append to the end of the bucket
        __hexfont_node *iter = (__hexfont_node *)font->buckets[node->key];
        while (iter->next) {
            iter = iter->next;
        }
        iter->next = node;
    }
}

