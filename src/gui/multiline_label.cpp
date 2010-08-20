#include "gui/multiline_label.h"

using namespace Glib;

MultilineLabel::MultilineLabel (const ControlParameters& parms):
Control(parms) {
}

void MultilineLabel::init_control (Control* par) {
    Control::init_control (par);
    set_frame (C_BACKGROUND);
}

void MultilineLabel::paint () {
    fill_backgound (get_background ());
    draw_wrapped_text (1, 1, get_width () - 2, get_height () - 2, get_text ());
}

void MultilineLabel::set_text (const ustring& value) {
    text = value;
    invalidate ();
}

const ustring& MultilineLabel::get_text () const {
    return text;
}

MultilineLabel* MultilineLabelFactory::create (Control* par, const ustring& text,
        const ControlParameters& parms, const ustring& name) {
    MultilineLabel* result = new MultilineLabel(parms);
    result->set_name (name);
    result->init_control (par);
    result->set_text (text);
    return result;
}

bool MultilineLabel::is_focusable () const {
    return false;
}