#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <ibus.h>
#include <signal.h>
#include "tieng.h"
#include "am_giua.h"
#include "engine.h"


#define IBUS_TELEX_ENGINE_NAME "telex"
#define IBUS_TELEX_ENGINE_LONGNAME "Dream Telex Input Method"
#define IBUS_TELEX_ENGINE_DESC "Dreamtiano's Telex Input Method Engine"
#define IBUS_TELEX_ENGINE_AUTHOR "Dreamtiano"
#define IBUS_TELEX_ENGINE_TEXTDOMAIN "ibus-telex-vn"
#define IBUS_TELEX_ENGINE_ICON ""
#define IBUS_TELEX_ENGINE_LAYOUT "us"

static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;
static IBusComponent *component = NULL;

static void
bus_disconnected_cb(IBusBus *bus, gpointer user_data)
{
    printf("Telex Engine: Bus disconnected\n");
    ibus_quit();
}

static void
init_ibus(void)
{
    IBusComponent *component;
    IBusEngineDesc *engine_desc;
    
    bus = ibus_bus_new();
    g_signal_connect(bus, "disconnected", G_CALLBACK(bus_disconnected_cb), NULL);
    
    // Create component
    component = ibus_component_new(
        "org.freedesktop.IBus.TelexVN",                 // component name
        IBUS_TELEX_ENGINE_DESC,                         // description
        "1.0.0",                                        // version
        "GPL",                                          // license
        IBUS_TELEX_ENGINE_AUTHOR,                       // author
        "https://github.com/yourusername/ibus-telex",   // homepage
        "/usr/lib/ibus/ibus-telex-vn",                  // command line
        IBUS_TELEX_ENGINE_TEXTDOMAIN                    // textdomain
    );
    
    // Create engine description
    engine_desc = ibus_engine_desc_new(
        IBUS_TELEX_ENGINE_NAME,                         // engine name
        IBUS_TELEX_ENGINE_LONGNAME,                     // long name
        IBUS_TELEX_ENGINE_DESC,                         // description
        "vi",                                           // language
        "GPL",                                          // license
        IBUS_TELEX_ENGINE_AUTHOR,                       // author
        IBUS_TELEX_ENGINE_ICON,                         // icon
        "us"                                            // layout
    );
    
    ibus_component_add_engine(component, engine_desc);
    
    factory = ibus_factory_new(ibus_bus_get_connection(bus));
    
    ibus_factory_add_engine(factory, IBUS_TELEX_ENGINE_NAME, IBUS_TELEX_TYPE_ENGINE);
    
    if (ibus_bus_register_component(bus, component)) {
        printf("TelexEngine: Component registered successfully\n");
    } else {
        printf("TelexEngine: Failed to register component\n");
        exit(1);
    }

    if (ibus_bus_request_name(bus, "org.freedesktop.IBus.TelexVN", 0)) {
        printf("TelexEngine: Name requested successfully\n");
    } else {
        printf("TelexEngine: Failed to request name\n");
        exit(1);
    }
}


int main(int argc, char *argv[])
{
    ibus_init();
    
    printf("TelexEngine: Starting IBus Telex Engine\n");
    
    init_ibus();
    
    printf("TelexEngine: Engine ready, entering main loop\n");
    
    ibus_main();
    
    printf("TelexEngine: Exiting\n");
    
    return 0;

}
