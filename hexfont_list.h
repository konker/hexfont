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

#ifndef __HEXFONT_LIST_H__
#define __HEXFONT_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hexfont.h"

/**
 * A singly linked list of font/font_metrics pairs
  */
typedef struct hexfont_list {
    hexfont *font;
    struct hexfont_list *next;

} hexfont_list;

hexfont_list * const hexfont_list_create(hexfont * const font);
void hexfont_list_destroy(hexfont_list * const head);
void hexfont_list_append(hexfont_list * const head, hexfont * const new_item);

uint16_t hexfont_list_get_length(hexfont_list * const head);
hexfont * const hexfont_list_get_nth(hexfont_list * const head, int16_t n);

#ifdef __cplusplus
}
#endif

#endif // __HEXFONT_LIST_H__

