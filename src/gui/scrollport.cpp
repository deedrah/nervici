#include "logger.h"
#include "gui/defaults.h"
#include "gui/view.h"

#include "gui/scrollport.h"

using namespace Glib;

ControlParameters Scrollport::view_parms = {
    0.0,
    0.0,
    0.0,
    0.0,
    0.1};

Scrollport::Scrollport (const ScrollbarParameters& parms) :
    Control (parms), view (NULL), content (NULL), bar (NULL),
            bar_parms (ScrollbarParameters (0,
                0,
                15,
                parms.h,
                10,
                parms.small_step,
                parms.big_step)) {
}

void Scrollport::init_control (Control* par) {
    view = ViewFactory::create (this, NULL, view_parms);
    bar = ScrollbarFactory::create (this, bar_parms);
    Control::init_control (par);
    bar->register_on_value_changed (Scrollbar::OnValueChanged (this,
        &Scrollport::bar_value_changed));
    bar->register_on_focus_gained (OnFocusGained (this,
        &Scrollport::child_focus_gained));
    bar->register_on_focus_lost (OnFocusLost (this,
        &Scrollport::child_focus_lost));
    /* view->show_all ();
     bar->show_all ();*/
}

Scrollport* ScrollportFactory::create (Control* par,
        Control* content,
        const ScrollbarParameters& parms,
        const ustring& name) {
    Scrollport* result = new Scrollport (parms);
    result->set_name (name);
    result->init_control (par);
    result->set_content (content);
    return result;
}

void Scrollport::reinitialize () {
    Control::reinitialize ();
    view->set_x (1);
    view->set_y (1);
}

void Scrollport::set_width (int value) {
    Control::set_width (value);
    if (bar->is_visible ()) {
        view->set_width (get_width () - bar->get_width () - 1);
    } else {
        view->set_width (get_width ());
    }
    if (view->get_content () != NULL) {
        view->get_content ()->set_width (view->get_width ());
    }
    bar->set_x (get_width () - bar->get_width ());
}

void Scrollport::set_height (int value) {
    Control::set_height (value);
    view->set_height (get_height () - 2);
    bar->set_height (get_height ());
    if (view->get_content () != NULL) {
        bar->set_max (view->get_content()->get_height() - get_height ());
        bar->set_visible (view->get_height ()
                < view->get_content ()->get_height ());
        set_width (get_width ());
    } else {
        bar->set_visible (false);
    }
}

void Scrollport::bar_value_changed (Scrollbar* ctl, int value) {
    content->set_y (1 - value);
    invalidate ();
}

void Scrollport::content_height_changed (Control* ctl, int value) {
    bar->set_max (value - get_height ());
    bar->set_visible (view->get_height ()
            < view->get_content ()->get_height ());
    set_width (get_width ());
}

void Scrollport::set_content (Control* value) {
    if (content != value) {
        if (content != NULL) {
            content->register_on_height_changed (OnHeightChanged ());
            content->register_on_focus_gained (OnFocusGained ());
            content->register_on_focus_lost (OnFocusLost ());

            view->set_content (NULL);

            bar->set_min (0);
            bar->set_value (0);
            bar->set_max (0);
        }
        content = value;

        if (content != NULL) {
            bar->set_min (0);
            bar->set_max (content->get_height () - view->get_height ());
            bar->set_value (0);

            view->set_content (content);

            content->register_on_height_changed (OnHeightChanged (this,
                &Scrollport::content_height_changed));
            content->register_on_focus_gained (OnFocusGained (this,
                &Scrollport::child_focus_gained));
            content->register_on_focus_lost (OnFocusLost (this,
                &Scrollport::child_focus_lost));
        }
    }
}

Control* Scrollport::get_content () const {
    return content;
}

void Scrollport::child_focus_gained (Control* ctl) {
    set_frame (NC_FOCUSED);
}

void Scrollport::child_focus_lost (Control* ctl) {
    set_frame (NC_HIGHGROUND);
}

bool Scrollport::is_focusable () const {
    return false;
}
