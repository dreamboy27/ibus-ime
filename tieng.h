#ifndef TIENG_H
#define TIENG_H

#include "am_giua.h"

typedef struct {
    GString *am_dau;
    AmGiua *am_giua;
    GString *am_cuoi;
    bool valid;
    GString *word;
    int cur;
} Tieng;

Tieng* tieng_new(const GString *am_dau, AmGiua *am_giua, const GString *am_cuoi);

Tieng* tieng_clone(const Tieng *src);

void tieng_free(Tieng *tieng);

void tieng_append(Tieng *tieng, gchar c);

void tieng_backspace(Tieng *tieng);

gchar* tieng_to_string(Tieng *tieng);

#endif