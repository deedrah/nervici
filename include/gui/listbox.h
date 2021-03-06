/* 
 * File:   listbox.h
 * Author: bobo
 *
 * Created on 11. červenec 2010, 15:48
 */

#ifndef LISTBOX_H
#define	LISTBOX_H

#include <vector>

#include "input_control.h"
#include "listbox_parameters.h"
#include "list_item.h"

class Listbox: public InputControl {
private:
    std::vector<ListItem> items;
    
    int selected;
    
    int item_height;
    
    const ListboxParameters lb_parms;

protected:
    Listbox (const ListboxParameters& parms);

    void init_control (Control* par);

    const ListboxParameters& get_parms ();

    virtual void select_up ();

    virtual void select_down ();

    virtual void paint ();

    void process_mouse_move_event (const SDL_MouseMotionEvent& event);

    bool process_key_pressed_event (const SDL_KeyboardEvent& event);

    void reinitialize ();

public:

    virtual void clear ();

    virtual void add_item (const Glib::ustring& text, uint32_t color =
            C_INPUT_TEXT);

    virtual const ListItem& get_item (int index);

    virtual int get_items_count () const;

    virtual void update_item (int index,
            const Glib::ustring& text,
            uint32_t color = C_INPUT_TEXT);

    virtual void remove_item (int index);

    virtual void insert_item (int index,
            const Glib::ustring& text,
            uint32_t color = C_INPUT_TEXT);

    virtual void set_selected (int value);

    virtual int get_selected () const;

    virtual void set_item_height (int value);

    virtual int get_item_height () const;

    friend class ListboxFactory;

};

class ListboxFactory {
public:
    static Listbox* create (Control* par,
            const ListboxParameters& parms,
            const Glib::ustring& name = "listbox");
};

#endif	/* LISTBOX_H */

