#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "am_giua.h"

static GHashTable *nguyen_am_goc_chuan = NULL;
static void init_validation_tables() {
    // nguyen_am_goc_chuan
    nguyen_am_goc_chuan = g_hash_table_new(g_str_hash, g_str_equal);
    const char *nguyen_am_list[] = {
        // nguyen am don
        "a", "i", "y", "o", "e", "u",
        // nguyen am doi
        "ai", "ao", "au", "ay", "ia", "ie", "ye", "eu", "eo", "iu", "oa", "oe", "oi", "oo",
        "ua", "ue", "ui", "uo", "uu", "uy",
        // nguyen am ba
        "oai", "oao", "uao", "oeo", "ieu", "yeu", "uoi", "uou", "uyu", "uye", "oay", "uay"
    };
    for (int i = 0; i < G_N_ELEMENTS(nguyen_am_list); i++) {
        g_hash_table_insert(nguyen_am_goc_chuan, (gpointer)nguyen_am_list[i], GINT_TO_POINTER(1));
    }
}

static void cleanup_validation_tables() {
    if (nguyen_am_goc_chuan) g_hash_table_destroy(nguyen_am_goc_chuan);
}

AmGiua* am_giua_new(const GString *nguyen_am_goc, const GString *am_cuoi) {
    AmGiua *am_giua = g_malloc(sizeof(AmGiua));
    am_giua->nguyen_am_goc = g_string_new(nguyen_am_goc->str);
    am_giua->dau_am = '\0';
    am_giua->thanh = '\0';
    am_giua->valid = true;
    am_giua->append = g_string_new("");
    am_giua->am_cuoi = g_string_new(am_cuoi->str);
    am_giua->w_index = g_array_new(FALSE, FALSE, sizeof(int));
    return am_giua;
}

AmGiua* am_giua_copy(AmGiua *other) {
    AmGiua *am_giua = g_malloc(sizeof(AmGiua));
    am_giua->nguyen_am_goc = g_string_new(other->nguyen_am_goc->str);
    am_giua->dau_am = other->dau_am;
    am_giua->thanh = other->thanh;
    am_giua->valid = other->valid;
    am_giua->append = g_string_new(other->append->str);
    am_giua->am_cuoi = g_string_new(other->am_cuoi->str);
    am_giua->w_index = g_array_new(FALSE, FALSE, sizeof(int));
    for (int i = 0; i < other->w_index->len; i++) {
        int val = g_array_index(other->w_index, int, i);
        g_array_append_val(am_giua->w_index, val);
    }
    return am_giua;
}

void am_giua_free(AmGiua *am_giua) {
    if (am_giua) {
        g_string_free(am_giua->nguyen_am_goc, TRUE);
        g_string_free(am_giua->am_cuoi, TRUE);
        g_string_free(am_giua->append, TRUE);
        g_array_free(am_giua->w_index, TRUE);
        g_free(am_giua);
    }
}

void am_giua_set_am_cuoi(AmGiua *am_giua, const GString *am_cuoi) {
    g_free(am_giua->am_cuoi);
    am_giua->am_cuoi = g_string_new(am_cuoi->str);
}

bool am_giua_is_valid(AmGiua *am_giua) {
    init_validation_tables();
    return g_hash_table_contains(nguyen_am_goc_chuan, am_giua->nguyen_am_goc->str);
}

void am_giua_add_char(AmGiua *am_giua, gchar c) {
    if (am_giua->valid) {
        switch (c) {
            case 'a': am_giua_handle_a_key(am_giua); break;
            case 'o': am_giua_handle_o_key(am_giua); break;
            case 'e': am_giua_handle_e_key(am_giua); break;
            case 'w': am_giua_handle_w_key(am_giua); break;
            case 's': am_giua_handle_s_key(am_giua, am_giua->am_cuoi); break;
            case 'f': am_giua_handle_f_key(am_giua, am_giua->am_cuoi); break;
            case 'r': am_giua_handle_r_key(am_giua, am_giua->am_cuoi); break;
            case 'x': am_giua_handle_x_key(am_giua, am_giua->am_cuoi); break;
            case 'j': am_giua_handle_j_key(am_giua, am_giua->am_cuoi); break;
            default: am_giua_handle_normal_key(am_giua, c); break;
        }
    }
}

void am_giua_handle_normal_key(AmGiua *am_giua, gchar c) {
    if (c == 'w') {
        if (strlen(am_giua->nguyen_am_goc->str) == 0) {
            gchar *new_str = g_strdup_printf("%su", am_giua->nguyen_am_goc->str);
            g_string_free(am_giua->nguyen_am_goc, TRUE);
            am_giua->nguyen_am_goc = g_string_new(new_str);
            g_free(new_str);
            am_giua->dau_am = 'w';
            int index = 1;
            g_array_append_val(am_giua->w_index, index);
        } else {
            g_string_append_c(am_giua->append, 'w');
        }
    } else {
        if (am_giua->am_cuoi->len > 0) {
            am_giua->append = g_string_append_c(am_giua->append, c);
        } else {
            gchar *new_str = g_strdup_printf("%s%c", am_giua->nguyen_am_goc->str, c);
            g_string_free(am_giua->nguyen_am_goc, TRUE);
            am_giua->nguyen_am_goc = g_string_new(new_str);
            g_free(new_str);
        }
    }
}

void am_giua_handle_a_key(AmGiua *am_giua) {
    for (int i = 0; i < strlen(am_giua->nguyen_am_goc->str); i++) {
        if (am_giua->nguyen_am_goc->str[i] == 'a') {
            am_giua_add_dau_am(am_giua, 'a');
            return;
        }
    }
    am_giua_handle_normal_key(am_giua, 'a');
}

void am_giua_handle_e_key(AmGiua *am_giua) {
    for (int i = 0; i < strlen(am_giua->nguyen_am_goc->str); i++) {
        if (am_giua->nguyen_am_goc->str[i] == 'e') {
            am_giua_add_dau_am(am_giua, 'e');
            return;
        }
    }
    am_giua_handle_normal_key(am_giua, 'e');
}

void am_giua_handle_o_key(AmGiua *am_giua) {
    for (int i = 0; i < strlen(am_giua->nguyen_am_goc->str); i++) {
        if (am_giua->nguyen_am_goc->str[i] == 'o') {
            am_giua_add_dau_am(am_giua, 'o');
            return;
        }
    }
    am_giua_handle_normal_key(am_giua, 'o');
}

void am_giua_handle_w_key(AmGiua *am_giua) {
    int last_index = (am_giua->w_index->len == 0) ? -1 : g_array_index(am_giua->w_index, int, am_giua->w_index->len - 1);
    int cur_index = strlen(am_giua->nguyen_am_goc->str);
    g_array_append_val(am_giua->w_index, cur_index);
    
    if (am_giua->w_index->len >= 3) {
        am_giua->dau_am = '\0';
        g_string_append_c(am_giua->append, 'w');
        return;
    }
    
    if (last_index == -1) {
        for (int i = 0; i < strlen(am_giua->nguyen_am_goc->str); i++) {
            if (am_giua->nguyen_am_goc->str[i] == 'a' || am_giua->nguyen_am_goc->str[i] == 'u' ||
                am_giua->nguyen_am_goc->str[i] == 'o') {
                am_giua->dau_am = 'w';
                return;
            }
        }
        am_giua_handle_normal_key(am_giua, 'w');
    } else {
        for (int i = cur_index - 1; i >= last_index; i--) {
            if (am_giua->nguyen_am_goc->str[i] == 'o') {
                am_giua->dau_am = 'w';
                return;
            }
        }
        am_giua->dau_am = '\0';
        g_string_append_c(am_giua->append, 'w');
    }
}

void am_giua_add_dau_am(AmGiua *am_giua, gchar c) {
    if (am_giua->dau_am != c) {
        am_giua->dau_am = c;
    } else {
        am_giua->dau_am = '\0';
        g_string_append_c(am_giua->append, c);
    }
}

void am_giua_handle_s_key(AmGiua *am_giua, GString *am_cuoi) {
    if (am_giua->thanh != 's') {
            am_giua->thanh = 's';
        }
    else {
        am_giua->thanh = '\0';
        g_string_append_c(am_giua->append, 's');
    }
}

void am_giua_handle_r_key(AmGiua *am_giua, GString *am_cuoi) {
    if (!(g_strcmp0(am_cuoi->str, "p") == 0 ||
      g_strcmp0(am_cuoi->str, "t") == 0 ||
      g_strcmp0(am_cuoi->str, "c") == 0 ||
      g_strcmp0(am_cuoi->str, "ch") == 0)) {
    
        if (am_giua->thanh != 'r') {
            am_giua->thanh = 'r';
        } else {
            am_giua->thanh = '\0';
            g_string_append_c(am_giua->append, 'r');
        }
    } 
    else {
        g_string_append_c(am_giua->append, 'r');
    }
}

void am_giua_handle_f_key(AmGiua *am_giua, GString *am_cuoi) {
    if (!(g_strcmp0(am_cuoi->str, "p") == 0 ||
      g_strcmp0(am_cuoi->str, "t") == 0 ||
      g_strcmp0(am_cuoi->str, "c") == 0 ||
      g_strcmp0(am_cuoi->str, "ch") == 0)) {
    
        if (am_giua->thanh != 'f') {
            am_giua->thanh = 'f';
        } else {
            am_giua->thanh = '\0';
            g_string_append_c(am_giua->append, 'f');
        }
    } 
    else {
        g_string_append_c(am_giua->append, 'f');
    }
}

void am_giua_handle_x_key(AmGiua *am_giua, GString *am_cuoi) {
    if (!(g_strcmp0(am_cuoi->str, "p") == 0 ||
      g_strcmp0(am_cuoi->str, "t") == 0 ||
      g_strcmp0(am_cuoi->str, "c") == 0 ||
      g_strcmp0(am_cuoi->str, "ch") == 0)) {
    
        if (am_giua->thanh != 'x') {
            am_giua->thanh = 'x';
        } else {
            am_giua->thanh = '\0';
            g_string_append_c(am_giua->append, 'x');
        }
    } 
    else {
        g_string_append_c(am_giua->append, 'x');
    }
}

void am_giua_handle_j_key(AmGiua *am_giua, GString *am_cuoi) {
    if (am_giua->thanh != 'j') {
            am_giua->thanh = 'j';
        }
    else {
        am_giua->thanh = '\0';
        g_string_append_c(am_giua->append, 'j');
    }
}


gchar** am_giua_get_am_giua(AmGiua *am_giua) {
    GString *res = g_string_new(am_giua->nguyen_am_goc->str);
    int index = 0;

    if (am_giua->dau_am == 'a') {
        for (int i = 0; i < g_utf8_strlen(res->str, -1); i++) {
            if (gstring_utf8_char_equal(res, i, 'a')) {
                gstring_utf8_char_set(res, i, g_utf8_get_char("â"));
                index = i;
            }
        }
    }

    if (am_giua->dau_am == 'o') {
        for (int i = 0; i < g_utf8_strlen(res->str, -1); i++) {
            if (gstring_utf8_char_equal(res, i, 'o')) {
                gstring_utf8_char_set(res, i, g_utf8_get_char("ô"));
                index = i;
            }
        }
    }

    if (am_giua->dau_am == 'e') {
        for (int i = 0; i < g_utf8_strlen(res->str, -1); i++) {
            if (gstring_utf8_char_equal(res, i, 'e')) {
                gstring_utf8_char_set(res, i, g_utf8_get_char("ê"));
                index = i;
            }
        }
    }

    if (am_giua->dau_am == 'w') {
        const gchar *str = res->str;
        if (g_strcmp0(str, "a") == 0) {
            g_string_assign(res, "ă");
        } else if (g_strcmp0(str, "o") == 0) {
            g_string_assign(res, "ơ");
        } else if (g_strcmp0(str, "u") == 0) {
            g_string_assign(res, "ư");
        } else if (g_strcmp0(str, "uo") == 0) {
            g_string_assign(res, "ươ");
        } else if (g_strcmp0(str, "ua") == 0) {
            g_string_assign(res, "ưa");
        } else if (g_strcmp0(str, "oa") == 0) {
            g_string_assign(res, "oă");
        } else if (g_strcmp0(str, "uu") == 0) {
            g_string_assign(res, "ưu");
        } else if (g_strcmp0(str, "uou") == 0) {
            g_string_assign(res, "ươu");
        } else if (g_strcmp0(str, "uoi") == 0) {
            g_string_assign(res, "ươi");
        } else {
            am_giua->append = g_string_append(am_giua->append, "w");
        }
    }

    // Tìm vị trí nguyên âm chính nếu thanh điệu là '0' hoặc 'w'
    if (am_giua->dau_am != '\0' && am_giua->dau_am != 'w') {
        // Với thanh điệu s, f, r, x, j trên â, ê, ô
        switch (am_giua->thanh) {
            case 's': 
                if (gstring_utf8_char_equal(res, index, g_utf8_get_char("â")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ấ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ê")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ế"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ô")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ố"));
                break;
            case 'f':
                if (gstring_utf8_char_equal(res, index, g_utf8_get_char("â")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ầ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ê")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ề"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ô")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ồ"));
                break;

            case 'r':
                if (gstring_utf8_char_equal(res, index, g_utf8_get_char("â")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ẩ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ê")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ể"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ô")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ổ"));
                break;
            
            case 'x':
                if (gstring_utf8_char_equal(res, index, g_utf8_get_char("â")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ẫ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ê")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ễ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ô")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỗ"));
                break;

            case 'j':
                if (gstring_utf8_char_equal(res, index, g_utf8_get_char("â")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ậ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ê")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ệ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ô")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ộ"));
                break;

        }
    } else {
        index = (g_utf8_strlen(am_giua->nguyen_am_goc->str, -1) + g_utf8_strlen(am_giua->am_cuoi->str, -1) - 1) / 2;
        if (index >= g_utf8_strlen(am_giua->nguyen_am_goc->str, -1)) index = g_utf8_strlen(am_giua->nguyen_am_goc->str, -1) - 1;

        switch (am_giua->thanh) {
            case 's':
                if (gstring_utf8_char_equal(res, index, 'a'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("á"));
                else if (gstring_utf8_char_equal(res, index, 'e')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("é"));
                else if (gstring_utf8_char_equal(res, index, 'o')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ó"));
                else if (gstring_utf8_char_equal(res, index, 'i')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("í"));
                else if (gstring_utf8_char_equal(res, index, 'u')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ú"));
                else if (gstring_utf8_char_equal(res, index, 'y')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ý"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ă"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ắ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ơ"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ớ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ư"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ứ"));
                break;
            
            case 'f':
                if (gstring_utf8_char_equal(res, index, 'a')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("à"));
                else if (gstring_utf8_char_equal(res, index, 'e')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("è"));
                else if (gstring_utf8_char_equal(res, index, 'o')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ò"));
                else if (gstring_utf8_char_equal(res, index, 'i')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ì"));
                else if (gstring_utf8_char_equal(res, index, 'u')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ù"));
                else if (gstring_utf8_char_equal(res, index, 'y')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỳ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ă"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ằ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ơ"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ờ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ư"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ừ"));
                break;
            
            case 'r':
                if (gstring_utf8_char_equal(res, index, 'a'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ả"));
                else if (gstring_utf8_char_equal(res, index, 'e'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ẻ"));
                else if (gstring_utf8_char_equal(res, index, 'o'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỏ"));
                else if (gstring_utf8_char_equal(res, index, 'i'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỉ"));
                else if (gstring_utf8_char_equal(res, index, 'u'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ủ"));
                else if (gstring_utf8_char_equal(res, index, 'y'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỷ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ă")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ẳ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ơ")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ở"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ư")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ử"));
                break;
            
            case 'x':
                if (gstring_utf8_char_equal(res, index, 'a')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ã"));
                else if (gstring_utf8_char_equal(res, index, 'e')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ẽ"));
                else if (gstring_utf8_char_equal(res, index, 'o')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("õ"));
                else if (gstring_utf8_char_equal(res, index, 'i')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ĩ"));
                else if (gstring_utf8_char_equal(res, index, 'u')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ũ"));
                else if (gstring_utf8_char_equal(res, index, 'y')) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỹ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ă"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ẵ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ơ"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỡ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ư"))) 
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ữ"));
                break;
            
            case 'j':
                if (gstring_utf8_char_equal(res, index, 'a'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ạ"));
                else if (gstring_utf8_char_equal(res, index, 'e'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ẹ"));
                else if (gstring_utf8_char_equal(res, index, 'o'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ọ"));
                else if (gstring_utf8_char_equal(res, index, 'i'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ị"));
                else if (gstring_utf8_char_equal(res, index, 'u'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ụ"));
                else if (gstring_utf8_char_equal(res, index, 'y'))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ỵ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ă")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ặ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ơ")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ợ"));
                else if (gstring_utf8_char_equal(res, index, g_utf8_get_char("ư")))
                    gstring_utf8_char_set(res, index, g_utf8_get_char("ự"));
                break;

        }

    }

    gchar **result = g_new(gchar*, 3);
    result[0] = g_strdup(res->str);
    result[1] = g_strdup(am_giua->append->str);
    result[2] = NULL;

    g_string_free(res, TRUE);
    return result;
}


gboolean gstring_utf8_char_equal(GString *s, int index, gunichar ch) {
    const gchar *p = s->str;
    int i = 0;
    while (i < index && *p) {
        p = g_utf8_next_char(p);
        i++;
    }

    if (*p == '\0') return FALSE;

    gunichar current = g_utf8_get_char(p);
    return current == ch;
}


void gstring_utf8_char_set(GString *s, int index, gunichar ch) {
    const gchar *p = s->str;
    int i = 0;

    while (i < index && *p) {
        p = g_utf8_next_char(p);
        i++;
    }

    if (*p == '\0') {
        return;
    }

    const gchar *start = p;
    const gchar *end = g_utf8_next_char(p);

    gint start_offset = start - s->str;
    gint end_offset = end - s->str;

    gchar buf[6]; 
    gint len = g_unichar_to_utf8(ch, buf);
    buf[len] = '\0'; 

    g_string_erase(s, start_offset, end_offset - start_offset);
    g_string_insert(s, start_offset, buf);
}