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
#include <string.h>
#include "hexfont.h"
#include "hexfont_list.h"


hexfont_list * const hexfont_list_create(hexfont * const item) {
    // Allocate memory for the head element
    hexfont_list * const font_list = malloc(sizeof(hexfont_list));

    font_list->item = item;
    font_list->next = NULL;

    return font_list;
}

void hexfont_list_destroy(hexfont_list * const head) {
    if (head == NULL) {
        return;
    }

    hexfont_list *tmp, *iter = head;
    while (iter->next) {
        tmp = iter;
        iter = iter->next;

        hexfont_destroy(tmp->item);
        free(tmp);
    }
    hexfont_destroy(iter->item);
    free(iter);
}

void hexfont_list_append(hexfont_list * const head, hexfont * const new_item) {
    if (head == NULL) {
        return;
    }

    hexfont_list *tail = head;
    while (tail->next) {
        tail = tail->next;
    }

    if (tail->item == NULL) {
        tail->item = new_item;
    }
    else {
        tail->next = hexfont_list_create(new_item);
    }
}

uint16_t hexfont_list_get_length(hexfont_list * const head) {
    if (head == NULL) {
        return 0;
    }

    uint16_t ret = 0;
    hexfont_list *iter;
    for (iter=head; iter!=NULL; iter=iter->next) {
        ret++;
    }
    return ret;
}

hexfont * const hexfont_list_get_nth(hexfont_list * const head, int16_t n) {
    if (hexfont_list_get_length(head) <= n) {
        return NULL;
    }

    int16_t i;
    hexfont_list *iter = head;
    for (i=0; i<n; i++) {
        iter = iter->next;
    }
    return iter->item;
}

