/* 
 * File:   key_graber.h
 * Author: bobo
 *
 * Created on 2. srpen 2010, 21:57
 */

#ifndef KEY_GRABER_H
#define	KEY_GRABER_H

#include "button.h"

class KeyGraber : public Button {

private:
    int key;
    ustring text;

protected:
    KeyGraber (const ControlParameters& parms);

    bool process_key_pressed_event (SDL_KeyboardEvent event);

public:

    virtual int get_key () const;

    virtual void set_key (int value);

    void set_text (const ustring& value);
    
    const ustring& get_text () const;

    friend class KeyGraberFactory;
};

class KeyGraberFactory {
public:
    static KeyGraber* create (Control* parent, const ustring& text,
            const ControlParameters& parms, const ustring& name = "key_graber");

};



#endif	/* KEY_GRABER_H */

