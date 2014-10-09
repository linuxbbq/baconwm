#include <xcb/xcb.h>
/*int init_baconwm();*/

int init_baconwm () {

    uint32_t values[3];

    xcb_connection_t *disp;
    xcb_screen_t *screen;
    xcb_drawable_t win;
    xcb_drawable_t root;

    xcb_generic_event_t *ev;
    xcb_get_geometry_reply_t *geom;

    disp = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(disp)) return 1; /*kill if error on connection */

    screen = xcb_setup_roots_iterator(xcb_get_setup(disp)).data;
    root = screen->root;

    xcb_grab_key(disp, 1, root, XCB_MOD_MASK_2, XCB_NO_SYMBOL,
                 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

    xcb_grab_button(disp, 0, root, XCB_EVENT_MASK_BUTTON_PRESS |
                XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
                XCB_GRAB_MODE_ASYNC, root, XCB_NONE, 1, XCB_MOD_MASK_1);

    xcb_grab_button(disp, 0, root, XCB_EVENT_MASK_BUTTON_PRESS |
                XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
                XCB_GRAB_MODE_ASYNC, root, XCB_NONE, 3, XCB_MOD_MASK_1);
    xcb_flush(disp);

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
            xcb_query_pointer_reply_t *pointer;
            pointer = xcb_query_pointer_reply(disp, xcb_query_pointer(disp, root), 0);
            if (values[2] == 1) {/* move */
                geom = xcb_get_geometry_reply(disp, xcb_get_geometry(disp, win), NULL);
                values[0] = (pointer->root_x + geom->width > screen->width_in_pixels)?
                    (screen->width_in_pixels - geom->width):pointer->root_x;
                values[1] = (pointer->root_y + geom->height > screen->height_in_pixels)?
                    (screen->height_in_pixels - geom->height):pointer->root_y;
                xcb_configure_window(disp, win, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
                xcb_flush(dpy);
            } else if (values[2] == 3) { /* resize */
                geom = xcb_get_geometry_reply(disp, xcb_get_geometry(dpy, win), NULL);
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


int main () {
    init_baconwm ();

    return 0;
}
