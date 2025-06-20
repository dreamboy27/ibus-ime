#ifndef AM_GIUA_H
#define AM_GIUA_H
#include <locale.h> 
#include <glib.h>
#include <stdbool.h>


typedef struct {
    GString *nguyen_am_goc;  // nguyên âm không có dấu 
    gchar dau_am;          // a, e, o, w hoặc '\0'
    gchar thanh;           // s, f, r, x, j hoặc '\0'
    bool valid;
    GString *append;
    GString *am_cuoi;
    GArray *w_index;
} AmGiua;


AmGiua* am_giua_new(const GString *nguyen_am_goc, const GString *am_cuoi);
AmGiua* am_giua_copy(AmGiua *other);
void am_giua_free(AmGiua *am_giua);

void am_giua_set_am_cuoi(AmGiua *am_giua, const GString *am_cuoi);
bool am_giua_is_valid(AmGiua *am_giua);

void am_giua_add_char(AmGiua *am_giua, gchar c);

void am_giua_handle_normal_key(AmGiua *am_giua, gchar c);
void am_giua_handle_a_key(AmGiua *am_giua);
void am_giua_handle_e_key(AmGiua *am_giua);
void am_giua_handle_o_key(AmGiua *am_giua);
void am_giua_handle_w_key(AmGiua *am_giua);
void am_giua_add_dau_am(AmGiua *am_giua, gchar c);

void am_giua_handle_s_key(AmGiua *am_giua, GString *am_cuoi);
void am_giua_handle_r_key(AmGiua *am_giua, GString *am_cuoi);
void am_giua_handle_f_key(AmGiua *am_giua, GString *am_cuoi);
void am_giua_handle_x_key(AmGiua *am_giua, GString *am_cuoi);
void am_giua_handle_j_key(AmGiua *am_giua, GString *am_cuoi);


gchar** am_giua_get_am_giua(AmGiua *am_giua);

void gstring_utf8_char_set(GString *s, int char_index, gunichar ch);
gboolean gstring_utf8_char_equal(GString *s, int char_index, gunichar ch);

#endif
