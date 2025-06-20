#ifndef ENGINE_H
#define ENGINE_H

#include <ibus.h>
#include <glib.h>
#include "tieng.h"

G_BEGIN_DECLS

#define IBUS_TELEX_TYPE_ENGINE (ibus_telex_engine_get_type())
GType ibus_telex_engine_get_type(void);
#define IBUS_TELEX_ENGINE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), IBUS_TELEX_TYPE_ENGINE, IBusTelexEngine))
#define IBUS_TELEX_ENGINE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), IBUS_TELEX_TYPE_ENGINE, IBusTelexEngineClass))
#define IBUS_IS_TELEX_ENGINE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), IBUS_TELEX_TYPE_ENGINE))
#define IBUS_IS_TELEX_ENGINE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), IBUS_TELEX_TYPE_ENGINE))
#define IBUS_TELEX_ENGINE_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS((obj), IBUS_TELEX_TYPE_ENGINE, IBusTelexEngineClass))

typedef struct _IBusTelexEngine IBusTelexEngine;
typedef struct _IBusTelexEngineClass IBusTelexEngineClass;

struct _IBusTelexEngine {
    IBusEngine parent;

    /* Cursor position */
    gint cursor_pos;
    
    /* Telex processing */
    Tieng *tieng;
    gboolean enabled;
    gboolean in_composition;
};

struct _IBusTelexEngineClass {
    IBusEngineClass parent;
};



/* Engine methods */
gboolean ibus_telex_engine_process_key_event(IBusEngine *engine,
                                           guint keyval,
                                           guint keycode,
                                           guint modifiers);

void ibus_telex_engine_focus_in(IBusEngine *engine);
void ibus_telex_engine_focus_out(IBusEngine *engine);
void ibus_telex_engine_reset(IBusEngine *engine);
void ibus_telex_engine_enable(IBusEngine *engine);
void ibus_telex_engine_disable(IBusEngine *engine);

void ibus_telex_engine_set_cursor_location(IBusEngine *engine,
                                         gint x,
                                         gint y,
                                         gint w,
                                         gint h);

void ibus_telex_engine_set_capabilities(IBusEngine *engine,
                                      guint caps);

void ibus_telex_engine_page_up(IBusEngine *engine);
void ibus_telex_engine_page_down(IBusEngine *engine);
void ibus_telex_engine_cursor_up(IBusEngine *engine);
void ibus_telex_engine_cursor_down(IBusEngine *engine);

/* Helper functions */
static void ibus_telex_engine_commit_text(IBusTelexEngine *telex);
static void ibus_telex_engine_commit_text_with_space(IBusTelexEngine *telex);
static void ibus_telex_engine_clear_buffer(IBusTelexEngine *telex);
static gboolean ibus_telex_engine_handle_backspace(IBusTelexEngine *telex);
static gboolean ibus_telex_engine_handle_character(IBusTelexEngine *telex, guint keyval);
static void ibus_telex_engine_init_word(IBusTelexEngine *telex);


G_END_DECLS

#endif