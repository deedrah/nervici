#include <cmath>
#include <clocale>
#include <glibmm/init.h>
#include <giomm/init.h>
#include <glibmm/thread.h>
#include <SDL.h>

#include "app.h"
#include "basic_defs.h"
#include "logger.h"

#include "main.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.141f
#endif

float icos[ANGLES];
float isin[ANGLES];

static void calc_angles () {
    for (int a = 0; a < ANGLES; a++) {
        icos[a] = floor (cos (M_PI * 2 * a / ANGLES) * DECIMALS) / DECIMALS;
        isin[a] = floor (sin (M_PI * 2 * a / ANGLES) * DECIMALS) / DECIMALS;
    }
}

int main (int argc, char *argv[]) {
    setlocale (LC_ALL, "");

    Glib::init ();
    Glib::thread_init ();
    Gio::init ();
    if (SDL_Init (0)) {
        logger.errln ("Could not initialize SDL library, halting.");
        return 1;
    }

    calc_angles ();

    if (app.initialize ()) {
        app.run ();
    }
    app.uninitialize ();

    SDL_Quit ();

    return 0;
}
