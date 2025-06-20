#include "tieng.h"


static GHashTable *am_dau_chuan = NULL;
static GHashTable *nguyen_am_goc_chuan = NULL;
static GHashTable *am_giua_chuan = NULL;
static GHashTable *am_cuoi_chuan = NULL;

static void init_validation_tables() {
    // Danh sách các âm đầu chuẩn chính tả
    am_dau_chuan = g_hash_table_new(g_str_hash, g_str_equal);
    const char *am_dau_list[] = {
        "b", "c", "k", "q", "qu", "d", "gi", "đ", "ch", "g", "gh", "ng", "ngh",
        "h", "nh", "l", "m", "n", "kh", "p", "ph", "r", "s", "t", "th", "tr", "v", "x"
    };
    for (int i = 0; i < G_N_ELEMENTS(am_dau_list); i++) {
        g_hash_table_insert(am_dau_chuan, (gpointer)am_dau_list[i], GINT_TO_POINTER(1));
    }

    // Danh sách nguyên âm không dấu 
    nguyen_am_goc_chuan = g_hash_table_new(g_str_hash, g_str_equal);
    const char *nguyen_am_list[] = {
        "a", "i", "y", "o", "e", "u",

        "ai", "ao", "au", "ay", "ia", "ie", "ye", "eu", "eo", "iu", "oa", "oe", "oi", "oo",
        "ua", "ue", "ui", "uo", "uu", "uy",

        "oai", "oao", "uao", "oeo", "ieu", "yeu", "uoi", "uou", "uyu", "uye", "oay", "uay"
    };
    for (int i = 0; i < G_N_ELEMENTS(nguyen_am_list); i++) {
        g_hash_table_insert(nguyen_am_goc_chuan, (gpointer)nguyen_am_list[i], GINT_TO_POINTER(1));
    }

    // Danh sách nguyên âm gốc 
    am_giua_chuan = g_hash_table_new(g_str_hash, g_str_equal);
    const char *am_giua_list[] = {
        "a", "ă", "â", "i", "y", "o", "ô", "ơ", "e", "ê", "u", "ư",

        "ai", "ao", "au", "ay", "âu", "ây", "ia", "iê", "yê", "êu", "eo", "iu", "oa", "oe", "oă", "oi", "oo", "ôô", "ơi",
        "ua", "ue", "uâ", "uă", "ui", "ưi", "uo", "ươ", "ưu", "uơ", "uy",

        "oai", "oao", "uao", "oeo", "iêu", "yêu", "uôi", "ươu", "uyu", "uyê", "oay", "uây", "ươi"
    };
    for (int i = 0; i < G_N_ELEMENTS(am_giua_list); i++) {
        g_hash_table_insert(am_giua_chuan, (gpointer)am_giua_list[i], GINT_TO_POINTER(1));
    }

    // 
    am_cuoi_chuan = g_hash_table_new(g_str_hash, g_str_equal);
    const char *am_cuoi_list[] = {
        "m", "p", "n", "t", "ng", "c", "nh", "ch",

        "â", "ê", "ô", "ơ", "ă", "ư",

        "ai", "ao", "au", "ay", "âu", "ây", "eo", "êu", "ia",
        "iu", "oi", "ôi", "ơi", "ưa", "ui", "ưi", "ưu", "uy",

        "iêu", "yêu", "uôi", "ươu", "uây", "ươi"
    };
    for (int i = 0; i < G_N_ELEMENTS(am_cuoi_list); i++) {
        g_hash_table_insert(am_cuoi_chuan, (gpointer)am_cuoi_list[i], GINT_TO_POINTER(1));
    }
}
static void cleanup_validation_tables() {
    if (am_dau_chuan) g_hash_table_destroy(am_dau_chuan);
    if (am_giua_chuan) g_hash_table_destroy(am_giua_chuan);
    if (am_cuoi_chuan) g_hash_table_destroy(am_cuoi_chuan);
}

Tieng* tieng_new(const GString *am_dau, AmGiua *am_giua, const GString *am_cuoi) {
    Tieng *tieng = g_malloc(sizeof(Tieng));
    tieng->am_dau = g_string_new(am_dau->str);
    tieng->am_giua = am_giua;
    tieng->am_cuoi = g_string_new(am_cuoi->str);
    tieng->valid = true;
    tieng->word = g_string_new("");
    tieng->cur = 0;
    init_validation_tables();
    return tieng;
}

Tieng* tieng_clone(const Tieng *src) {
    if (src == NULL) return NULL;

    Tieng *clone = g_new(Tieng, 1);

    clone->am_dau = g_string_new_len(src->am_dau->str, src->am_dau->len);
    clone->am_cuoi = g_string_new_len(src->am_cuoi->str, src->am_cuoi->len);
    clone->word = g_string_new_len(src->word->str, src->word->len);

    clone->am_giua = am_giua_copy(src->am_giua);

    clone->valid = src->valid;
    clone->cur = src->cur;

    return clone;
}


void tieng_free(Tieng *tieng) {
    if (tieng) {
        g_string_free(tieng->am_dau, TRUE);
        g_string_free(tieng->am_cuoi, TRUE);
        am_giua_free(tieng->am_giua);
        g_string_free(tieng->word, TRUE);
        cleanup_validation_tables();
        g_free(tieng);
    }
}

void tieng_append(Tieng *tieng, gchar c) {
    gchar sc[2] = { c, '\0' };
    g_string_append_c(tieng->word, c);
    
    if (tieng->valid) {
        if (tieng->cur == 0) {
            gchar *temp = g_strdup_printf("%s%c", tieng->am_dau->str, c);
            if (g_hash_table_contains(am_dau_chuan, temp) || 
                (g_strcmp0(tieng->am_dau->str, "d") == 0 && c == 'd')) {
                
                if (g_strcmp0(tieng->am_dau->str, "d") == 0 && c == 'd') {
                    g_string_free(tieng->am_dau, TRUE);
                    tieng->am_dau = g_string_new("đ");
                } else {
                    g_string_free(tieng->am_dau, TRUE);
                    tieng->am_dau = g_string_new(temp);
                    temp = NULL;
                }
            } else if (g_hash_table_contains(nguyen_am_goc_chuan, sc)) {
                am_giua_add_char(tieng->am_giua, c);
                tieng->cur = 1;
                if (temp) g_free(temp);
                return;
            } else {
                g_string_free(tieng->am_dau, TRUE);
                tieng->am_dau = g_string_new(temp);
                tieng->valid = false;
            }
            if (temp) g_free(temp);
        } else if (tieng->cur == 1) {
            if (strchr("aueoiywsfrxj", c)) {
                AmGiua *temp = am_giua_copy(tieng->am_giua);
                am_giua_add_char(temp, c);
                
                if (am_giua_is_valid(temp)) {
                    am_giua_add_char(tieng->am_giua, c);
                    gchar **str = am_giua_get_am_giua(tieng->am_giua);
                    if (strlen(str[1]) > 0) {
                        tieng->valid = false;
                    }
                    g_free(str[0]);
                    g_free(str[1]);
                    g_free(str);
                } else {
                    tieng->valid = false;
                }
                am_giua_free(temp);
            } else {
                if (g_strcmp0(tieng->am_dau->str, "d") == 0 && c == 'd') {
                    g_free(tieng->am_dau);
                    tieng->am_dau = g_string_new("đ");
                } else {
                    gchar *new_am_cuoi = g_strdup_printf("%s%c", tieng->am_cuoi->str, c);
                    g_free(tieng->am_cuoi);
                    tieng->am_cuoi = g_string_new(new_am_cuoi);
                    if (new_am_cuoi) g_free(new_am_cuoi);
                    am_giua_set_am_cuoi(tieng->am_giua, tieng->am_cuoi);
                    tieng->cur = 2;
                }
            }
        } else if (tieng->cur == 2) {
            if (strchr("aeoswfrxj", c)) {
                AmGiua *temp = am_giua_copy(tieng->am_giua);
                am_giua_add_char(temp, c);
                
                if (am_giua_is_valid(temp)) {
                    am_giua_add_char(tieng->am_giua, c);
                    gchar **str = am_giua_get_am_giua(tieng->am_giua);
                    if (strlen(str[1]) > 0) {
                        tieng->valid = false;
                    }
                    g_free(str[0]);
                    g_free(str[1]);
                    g_free(str);
                } else {
                    tieng->valid = false;
                }
                am_giua_free(temp);
            } else {
                if (g_strcmp0(tieng->am_dau->str, "d") == 0 && c == 'd') {
                    g_free(tieng->am_dau);
                    tieng->am_dau = g_string_new("đ");
                } else {
                    gchar *new_am_cuoi = g_strdup_printf("%s%c", tieng->am_cuoi->str, c);
                    g_free(tieng->am_cuoi);
                    tieng->am_cuoi = g_string_new(new_am_cuoi);
                    if (new_am_cuoi) g_free(new_am_cuoi);
                    am_giua_set_am_cuoi(tieng->am_giua, tieng->am_cuoi);
                }
            }
        }
    }
}

void tieng_backspace(Tieng *tieng) {
    if (tieng->valid == false) {
        g_string_truncate(tieng->word, tieng->word->len - 1);
        return;
    }
    if (tieng->cur == 2) {
        g_string_truncate(tieng->am_cuoi, tieng->am_cuoi->len - 1);
        am_giua_set_am_cuoi(tieng->am_giua, tieng->am_cuoi);
        if (tieng->am_cuoi->len == 0) {
            tieng->cur = 1;
        }
    } else if (tieng->cur == 1) {
        if (tieng->am_giua->nguyen_am_goc->len == 1) {
            g_string_truncate(tieng->am_giua->nguyen_am_goc, 0);
            g_string_free(tieng->am_giua->append, TRUE);
            tieng->am_giua->append = g_string_new("");
            tieng->am_giua->dau_am = '\0';
            tieng->am_giua->thanh = '\0';
            tieng->cur = 0;
        } 
        else {
            if (tieng->am_giua->nguyen_am_goc->str[tieng->am_giua->nguyen_am_goc->len - 1] 
                    == tieng->am_giua->dau_am) {
                g_string_truncate(tieng->am_giua->nguyen_am_goc, tieng->am_giua->nguyen_am_goc->len - 1);
                tieng->am_giua->dau_am = '\0';
            } 
            else if (tieng->am_giua->dau_am == 'w') {
                g_string_truncate(tieng->am_giua->nguyen_am_goc, tieng->am_giua->nguyen_am_goc->len - 1);
                tieng->am_giua->w_index = g_array_remove_index(tieng->am_giua->w_index, tieng->am_giua->w_index->len - 1);
            }
            else {
                g_string_truncate(tieng->am_giua->nguyen_am_goc, tieng->am_giua->nguyen_am_goc->len - 1);
            }
        }
    } else if (tieng->cur == 0) {
        if (tieng->am_dau->len > 0) {
            g_string_truncate(tieng->am_dau, tieng->am_dau->len - 1);
        }
    }
}


gchar* tieng_to_string(Tieng *tieng) {
    if (!tieng->valid) {
        return g_strdup(tieng->word->str);
    }
    gchar **am_giua_result = am_giua_get_am_giua(tieng->am_giua);
    gchar *result = g_strdup_printf("%s%s%s", tieng->am_dau->str, am_giua_result[0], tieng->am_cuoi->str);
    g_free(am_giua_result[0]);
    g_free(am_giua_result[1]);
    g_free(am_giua_result);
    
    return result;
}