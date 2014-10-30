#include <stdlib.h>
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <xcb/xcb_keysyms.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <X11/X.h>

/* Macros */
#define LENGTH(x) (unsigned int)(sizeof(x) / sizeof(*x))

/* union for keys */
typedef union { const char * const * const cmd;
    int i; /* may need later, we'll see. */
} Arg;

typedef struct { int mod; xcb_keysym_t sym;
    void (*func) (const Arg *);
    const Arg arg;
} Key;

/* xcb globals */
static void grab_keys(void); /* key grabbing function */
static xcb_keycode_t *keysym_to_keycode(xcb_keysym_t sym);
static void grab_keycode(xcb_keycode_t *keycode, const int mod);

/* general globals */
static xcb_connection_t *disp;
static xcb_screen_t *screen;
static void spawn(const Arg *arg);

#include "config.h"

int init_baconwm () {

    uint32_t values[3];

    xcb_drawable_t win;
    xcb_drawable_t root;

    xcb_generic_event_t *ev;
    xcb_get_geometry_reply_t *geom;

    disp = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(disp)) return 1; /*kill if error on connection */

    screen = xcb_setup_roots_iterator(xcb_get_setup(disp)).data;
    root = screen->root;

    grab_keys();

    xcb_flush(disp);

    /* big fucking loop */
    for (;;)
    {
        ev = xcb_wait_for_event(disp);
        switch (ev->response_type & ~0x80) {

        case XCB_BUTTON_PRESS:
        {
            xcb_button_press_event_t *e;
            e = ( xcb_button_press_event_t *) ev;
            win = e->child;
            values[0] = XCB_STACK_MODE_ABOVE;
            xcb_configure_window(disp, win, XCB_CONFIG_WINDOW_STACK_MODE, values);
            geom = xcb_get_geometry_reply(disp, xcb_get_geometry(disp, win), NULL);
            if (1 == e->detail) {
                values[2] = 1;
                xcb_warp_pointer(disp, XCB_NONE, win, 0, 0, 0, 0, 1, 1);
            } else {
                values[2] = 3;
                xcb_warp_pointer(disp, XCB_NONE, win, 0, 0, 0, 0, geom->width, geom->height);
            }
            xcb_grab_pointer(disp, 0, root, XCB_EVENT_MASK_BUTTON_RELEASE
                    | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION_HINT,
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, root, XCB_NONE, XCB_CURRENT_TIME);
            xcb_flush(disp);
        }
        break;

        case XCB_MOTION_NOTIFY:
        {
	    /* None of this matters, it's the tinywm stackable shizzle. */
	    /* I just have it here because it's cool.  We will snap all */
	    /* spawned windows to 0,0,w,h geometry for full-screen */
            xcb_query_pointer_reply_t *pointer;
            pointer = xcb_query_pointer_reply(disp, xcb_query_pointer(disp, root), 0);
            if (values[2] == 1) {/* move */
                geom = xcb_get_geometry_reply(disp, xcb_get_geometry(disp, win), NULL);
                values[0] = (pointer->root_x + geom->width > screen->width_in_pixels)?
                    (screen->width_in_pixels - geom->width):pointer->root_x;
                values[1] = (pointer->root_y + geom->height > screen->height_in_pixels)?
                    (screen->height_in_pixels - geom->height):pointer->root_y;
                xcb_configure_window(disp, win, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
                xcb_flush(disp);
            } else if (values[2] == 3) { /* resize */
                geom = xcb_get_geometry_reply(disp, xcb_get_geometry(disp, win), NULL);
                values[0] = pointer->root_x - geom->x;
                values[1] = pointer->root_y - geom->y;
                xcb_configure_window(disp, win, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
                xcb_flush(disp);
            }
        }
        break;

        case XCB_BUTTON_RELEASE:
            xcb_ungrab_pointer(disp, XCB_CURRENT_TIME);
            xcb_flush(disp);
        break;
        }
    }
    return 0;
}

void spawn(const Arg *arg){
    /* spawns a process via command */
    if (fork()) return;
    setsid();
    execvp((char *)arg->cmd[0], (char **)arg->cmd);
}

void grab_keys(void) {
    /* TODO: determine efficient way to capture keycode/keysyms */
    /* then, remove previous 'place-holder code' in main loop */
    int i;
    xcb_keycode_t *keycode;
    xcb_ungrab_key(disp, XCB_GRAB_ANY, screen->root, XCB_MOD_MASK_ANY);
    for (i=0;i<LENGTH(keys);i++) {
	keysym_to_keycode(keys[i].mod);
	grab_keycode(keycode, keys[i].mod);
    }
}

void grab_keycode(xcb_keycode_t *keycode, const int mod) {
    unsigned int x, k;
    uint16_t mods[] = {0, XCB_MOD_MASK_LOCK };
    for (x=0; keycode[x] != XCB_NO_SYMBOL; x++)
	for (k=0; k< LENGTH(mods); k++)
	    xcb_grab_key(disp, 1, screen->root, mod | mods[k], keycode[x],
			 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    free(keycode);
}

xcb_keycode_t *keysym_to_keycode(xcb_keysym_t sym) {
    xcb_keycode_t *code;
    xcb_key_symbols_t *syms = xcb_key_symbols_alloc(disp);

    if(!syms)
	return NULL;
    code = xcb_key_symbols_get_keycode(syms, sym);
    xcb_key_symbols_free(syms);
    return code;
    }

int main () {
    init_baconwm ();

    return 0;
}
