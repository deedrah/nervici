/* 
 * File:   combobox.h
 * Author: bobo
 *
 * Created on 14. červenec 2010, 16:27
 */

#ifndef COMBOBOX_H
#define	COMBOBOX_H

#include "defaults.h"
#include "input_control.h"
#include "listbox_parameters.h"
#include "scrollbar_parameters.h"
#include "fakes/gui/listbox.h"
#include "fakes/gui/list_item.h"
#include "fakes/gui/scrollport.h"

class Combobox: public InputControl {
public:

    typedef Event1<Combobox*, int> OnSelectedChanged;

private:
    Listbox* list;

    Scrollport* port;

    int selected;

    ListboxParameters list_parms;

    ScrollbarParameters port_parms;

    struct {
        std::list<OnSelectedChanged> selected_changed;
    } call;

    void list_clicked (Control* ctl);

    void select_up ();

    void select_down ();

protected:

    Combobox (const ListboxParameters& parms);

    void init_control (Control* par);

    void reinitialize ();

    void paint ();

    void on_clicked ();

    bool process_key_pressed_event (const SDL_KeyboardEvent& event);
    
    void process_mouse_button_event (const SDL_MouseButtonEvent& event);

    void on_focus_gained ();

    void on_focus_lost ();

    virtual void on_selected_changed (int sel);

public:

    ~Combobox ();

    virtual void clear ();

    virtual void add_item (const Glib::ustring& text, uint32_t color =
            NC_INPUT_TEXT);

    virtual const ListItem& get_item (int index);

    virtual int get_items_count () const;

    virtual void update_item (int index,
            const Glib::ustring& text,
            uint32_t color = NC_INPUT_TEXT);

    virtual void remove_item (int index);

    virtual void insert_item (int index,
            const Glib::ustring& text,
            uint32_t color = NC_INPUT_TEXT);

    virtual void set_selected (int value);

    virtual int get_selected () const;

    virtual void
    register_on_selected_changed (const OnSelectedChanged& handler);

    virtual void
    unregister_on_selected_changed (const OnSelectedChanged& handler);

    friend class ComboboxFactory;
};

class ComboboxFactory {
public:
    static Combobox* create (Control* par,
            const ListboxParameters& parms,
            const Glib::ustring& name = "combobox");
};

#endif	/* COMBOBOX_H */

