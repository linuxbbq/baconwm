#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>  /* the absolute LEAST to make a functional WM */

static Display *dis; /*declare X display*/
static int current_desktop; /* will be needed as active index in array */
