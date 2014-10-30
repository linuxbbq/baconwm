/* Like dwm/hcwm/catwm...etc.  Configure before build */

#ifndef CONFIG_H
#define CONFIG_H

#define MODKEY Mod1Mask /* alt */

static const char * const term_cmd[] = {"urxvt", NULL};

static const Key keys[] = {
    { MODKEY, XK_Return, spawn, {.cmd = term_cmd} },
};

#endif
