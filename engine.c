#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>



/* Forward declarations for GObject */
static void ibus_telex_engine_class_init(IBusTelexEngineClass *klass);
static void ibus_telex_engine_init(IBusTelexEngine *telex);
/* Function declarations */
static GObject*
ibus_telex_engine_constructor(GType                  type,
                             guint                  n_construct_params,
                             GObjectConstructParam *construct_params);
static void ibus_telex_engine_finalize(GObject *object);

/* Defines the IBusTelexEngine type and sets up the type system for GObject */
G_DEFINE_TYPE(IBusTelexEngine, ibus_telex_engine, IBUS_TYPE_ENGINE)

static gboolean is_our_backspace_key = FALSE;

/* Class initialization */
static void
ibus_telex_engine_class_init(IBusTelexEngineClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    IBusEngineClass *ibus_engine_class = IBUS_ENGINE_CLASS(klass);
    
    gobject_class->constructor = ibus_telex_engine_constructor;
    gobject_class->finalize = ibus_telex_engine_finalize;
    ibus_engine_class->process_key_event = ibus_telex_engine_process_key_event;
    ibus_engine_class->reset = ibus_telex_engine_reset;
    ibus_engine_class->enable = ibus_telex_engine_enable;
    ibus_engine_class->disable = ibus_telex_engine_disable;
    ibus_engine_class->focus_in = ibus_telex_engine_focus_in;
    ibus_engine_class->focus_out = ibus_telex_engine_focus_out;
    ibus_engine_class->set_cursor_location = ibus_telex_engine_set_cursor_location;
    ibus_engine_class->set_capabilities = ibus_telex_engine_set_capabilities;
    ibus_engine_class->page_up = ibus_telex_engine_page_up;
    ibus_engine_class->page_down = ibus_telex_engine_page_down;
    ibus_engine_class->cursor_up = ibus_telex_engine_cursor_up;
    ibus_engine_class->cursor_down = ibus_telex_engine_cursor_down;
}

/* Instance initialization */
static void
ibus_telex_engine_init(IBusTelexEngine *self)
{
    self->cursor_pos = 0;
        
    self->enabled = FALSE;
    self->in_composition = FALSE;

    ibus_telex_engine_init_word(self);
}

// Constructor Destructor
static void
ibus_telex_engine_finalize(GObject *object)
{
    IBusTelexEngine *telex_engine = IBUS_TELEX_ENGINE(object);
    
    // Giải phóng Telex processor
    if (telex_engine->tieng) {
        tieng_free(telex_engine->tieng);
        telex_engine->tieng = NULL;
    }
    
    g_debug("IBusTelexEngine: Engine finalized");
    
    // Gọi finalize của class cha
    G_OBJECT_CLASS(ibus_telex_engine_parent_class)->finalize(object);
}

static GObject*
ibus_telex_engine_constructor(GType                  type,
                             guint                  n_construct_params,
                             GObjectConstructParam *construct_params)
{
    IBusTelexEngine *telex_engine;
    GObject *object;
    
    // Gọi constructor của class cha
    object = G_OBJECT_CLASS(ibus_telex_engine_parent_class)->constructor(
        type, n_construct_params, construct_params);
    
    // Cast về IBusTelexEngine
    telex_engine = IBUS_TELEX_ENGINE(object);
    
    ibus_telex_engine_init(telex_engine);
    // Debug logging
    g_debug("IBusTelexEngine: Engine constructed successfully");
    
    return object;
}



/* Key event processing */
gboolean
ibus_telex_engine_process_key_event(IBusEngine *engine,
                                  guint keyval,
                                  guint keycode,
                                  guint modifiers)
{
    IBusTelexEngine *telex = (IBusTelexEngine *)engine;

    if (modifiers & IBUS_RELEASE_MASK)
        return FALSE;

    if (!telex->enabled)
        return FALSE;

    if (modifiers & (IBUS_CONTROL_MASK | IBUS_MOD1_MASK))
        return FALSE;

    switch (keyval) {
        case IBUS_KEY_space:
            if (telex->in_composition) {
                ibus_telex_engine_commit_text(telex);
                return TRUE;
            }
            return FALSE;

        case IBUS_KEY_Return:
        case IBUS_KEY_KP_Enter:
            if (telex->in_composition) {
                ibus_telex_engine_commit_text(telex);
                return TRUE;
            }
            return FALSE;

        // case IBUS_KEY_Escape:
        //     if (telex->in_composition) {
        //         ibus_telex_engine_clear_preedit_text(telex);
        //         return TRUE;
        //     }
        //     return FALSE;

        case IBUS_KEY_BackSpace:
            return FALSE;
    

        default:
            if (keyval >= IBUS_KEY_a && keyval <= IBUS_KEY_z) {
                return ibus_telex_engine_handle_character(telex, keyval);
            }
            else if (keyval >= IBUS_KEY_A && keyval <= IBUS_KEY_Z) {
                return ibus_telex_engine_handle_character(telex, keyval + 32); // Convert to lowercase
            }
            break;
    }

    if (telex->in_composition) {
        ibus_telex_engine_commit_text(telex);
        return FALSE;
    }

    return FALSE;
}



/* Engine state management */
void
ibus_telex_engine_focus_in(IBusEngine *engine)
{
    IBusTelexEngine *telex = (IBusTelexEngine *)engine;
    telex->enabled = TRUE;
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->focus_in(engine);
}

void
ibus_telex_engine_focus_out(IBusEngine *engine)
{
    IBusTelexEngine *telex = (IBusTelexEngine *)engine;
    if (telex->in_composition) {
        ibus_telex_engine_commit_text(telex);
    }
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->focus_out(engine);
}

void
ibus_telex_engine_reset(IBusEngine *engine)
{
    IBusTelexEngine *telex = (IBusTelexEngine *)engine;
    ibus_telex_engine_clear_buffer(telex);
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->reset(engine);
}

void
ibus_telex_engine_enable(IBusEngine *engine)
{
    IBusTelexEngine *telex = (IBusTelexEngine *)engine;
    telex->enabled = TRUE;
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->enable(engine);
}

void
ibus_telex_engine_disable(IBusEngine *engine)
{
    IBusTelexEngine *telex = (IBusTelexEngine *)engine;
    telex->enabled = FALSE;
    ibus_telex_engine_clear_buffer(telex);
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->disable(engine);
}



/* Cursor and capabilities */
void
ibus_telex_engine_set_cursor_location(IBusEngine *engine,
                                    gint x, gint y, gint w, gint h)
{
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->set_cursor_location(engine, x, y, w, h);
}

void
ibus_telex_engine_set_capabilities(IBusEngine *engine, guint caps)
{
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->set_capabilities(engine, caps);
}



/* Lookup table navigation (không cần dùng) */
void
ibus_telex_engine_page_up(IBusEngine *engine)
{
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->page_up(engine);
}

void
ibus_telex_engine_page_down(IBusEngine *engine)
{
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->page_down(engine);
}

void
ibus_telex_engine_cursor_up(IBusEngine *engine)
{
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->cursor_up(engine);
}

void
ibus_telex_engine_cursor_down(IBusEngine *engine)
{
    IBUS_ENGINE_CLASS(ibus_telex_engine_parent_class)->cursor_down(engine);
}




/* Helper functions */
static void
ibus_telex_engine_commit_text(IBusTelexEngine *telex)
{
    gchar *raw = tieng_to_string(telex->tieng);
    gchar *new_str = g_strconcat(raw, " ", NULL);
    gchar *buffer = g_utf8_normalize(new_str, -1, G_NORMALIZE_NFC);
    if (telex->tieng && g_utf8_strlen(buffer, - 1) > 0) {
        IBusText *text = ibus_text_new_from_string(buffer);
        ibus_engine_commit_text((IBusEngine *)telex, text);
    }
    g_free(buffer);
    g_free(raw);
    ibus_telex_engine_clear_buffer(telex);
}

static void ibus_telex_engine_update_preedit_text(IBusTelexEngine *telex) {
    gchar *raw = tieng_to_string(telex->tieng);
    gchar *buffer = g_utf8_normalize(raw, -1, G_NORMALIZE_NFC);
    if (telex->tieng && g_utf8_strlen(buffer, - 1) > 0) {
        IBusText *text = ibus_text_new_from_string(buffer);
    
        gchar *new_text = ibus_text_get_text(text);
        gchar *new_str = g_strconcat(new_text, " ", NULL);
        IBusText *new_text_obj = ibus_text_new_from_string(new_str);

        ibus_engine_update_preedit_text((IBusEngine *)telex, new_text_obj, 
                                       g_utf8_strlen(buffer, - 1), TRUE);
        ibus_engine_show_preedit_text((IBusEngine *)telex);
    }
    g_free(buffer);
    g_free(raw);
}

static void
ibus_telex_engine_clear_buffer(IBusTelexEngine *telex)
{
    telex->cursor_pos = 0;
    telex->in_composition = FALSE;
    ibus_telex_engine_init_word(telex);
}

// static gboolean
// ibus_telex_engine_handle_backspace(IBusTelexEngine *telex)
// {
//     ibus_telex_engine_clear_buffer(telex);
//     ibus_engine_forward_key_event((IBusEngine *)telex, IBUS_KEY_BackSpace, 14, 0);
//     return TRUE;
// }


static gboolean
ibus_telex_engine_handle_character(IBusTelexEngine *telex, guint keyval)
{
    gchar c = (gchar)keyval;
    telex->in_composition = TRUE;

    tieng_append(telex->tieng, c);
    ibus_telex_engine_update_preedit_text(telex);

    return TRUE;
}


static void
ibus_telex_engine_init_word(IBusTelexEngine *telex)
{
    if (telex->tieng) {
        tieng_free(telex->tieng);
    }
    
    GString *am_dau = g_string_new("");
    GString *am_cuoi = g_string_new("");
    GString *nguyen_am_goc = g_string_new("");
    
    AmGiua *am_giua = am_giua_new(nguyen_am_goc, am_cuoi);
    telex->tieng = tieng_new(am_dau, am_giua, am_cuoi);
    
    g_string_free(am_dau, TRUE);
    g_string_free(am_cuoi, TRUE);
}