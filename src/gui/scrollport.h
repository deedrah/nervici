/* 
 * File:   Scrollport.h
 * Author: bobo
 *
 * Created on 11. červenec 2010, 17:12
 */

#ifndef SCROLLPORT_H
#define	SCROLLPORT_H

#include "control.h"
#include "button.h"
#include "scrollbar.h"
#include "view.h"

class Scrollport : public Control {

private:
    View* view;
    Control* content;
    Scrollbar* bar;

    static ScrollbarParameters bar_parms;
    static ControlParameters view_parms;

    void bar_value_changed (Scrollbar* ctl, int value);

    void content_height_changed (Control* ctl, int value);

    void child_focus_gained (Control* ctl);

    void child_focus_lost (Control* ctl);

protected:
    Scrollport (const ControlParameters* parms);

    void init_control (Control* par);

    void reinitialize ();

public:
    static Scrollport* create_scrollport (Control* par, Control* content,
            const ControlParameters* parms, const ustring& name = "scrollport");

    bool is_focusable () const;

    void set_content (Control* value);

    Control* get_content () const;
};

#endif	/* SCROLLPORT_H */

