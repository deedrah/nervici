/*
 * sdl_sceen.h
 *
 *  Created on: 13.9.2010
 *      Author: bobo
 */

#ifndef SDL_SCREEN_H_
#define SDL_SCREEN_H_

#include "screen.h"
#include "fakes/sdl_decl.h"

class SDLScreen: public Screen {
private:
    SDL_Surface* off_face;
    
    SDL_Surface* primary;
    
    bool ignore_updates;
    
    bool abort;
    
protected:
    void reinitialize ();

    void update (Clip* scrvas);

    void process_event (const SDL_Event& event);

public:

    SDLScreen (SDL_Surface* primary);

    bool process_events ();
    
    void set_surface (SDL_Surface* value);
    
    void set_ignore_updates (bool value);

    bool is_aborted () const;

    friend class SDLScreenFactory;
};

class SDLScreenFactory {
public:
    static SDLScreen* create (SDL_Surface* primary);
};

#endif /* SDL_SCREEN_H_ */
