/* 
 * File:   pledit_frame.h
 * Author: bobo
 *
 * Created on 2. srpen 2010, 12:20
 */

#ifndef PLEDIT_FRAME_H
#define	PLEDIT_FRAME_H

#include "gui/frame.h"
#include "settings/pl_type.h"
#include "fakes/gui/combobox.h"
#include "fakes/gui/label.h"
#include "fakes/gui/textbox.h"
#include "fakes/gui/scale.h"
#include "fakes/gui/panel.h"
#include "fakes/gui/key_graber.h"
#include "fakes/gui/label_button.h"
#include "fakes/gui/checkbox.h"

class PlEditFrame: public Frame {
private:
    Combobox* cb_select;
    LabelButton* btn_del;
    Label* la_name;
    Textbox* te_name;
    Label* la_color;
    Scale* sa_red;
    Scale* sa_green;
    Scale* sa_blue;
    Panel* pa_color;
    Label* la_profil;
    Combobox* cb_profil;
    Label* la_pitch;
    Scale* sa_pitch;
    Label* la_ai;
    Combobox* cb_ai;
    Label* la_distance;
    Scale* sa_distance;
    Checkbox* cb_jump;

    Label* la_keys;
    KeyGraber* kg_left;
    KeyGraber* kg_right;
    KeyGraber* kg_jump;
    LabelButton* btn_back;
    PlTypes pl_type;

    int selected;

    void save_actual_player ();
    
    void on_btn_del_clicked (Control* ctl);

    void on_btn_back_clicked (Control* ctl);

    void on_cb_select_changed (Combobox* ctl, int value);

    void on_sa_color_changed (Scale* ctl, int value);

    void on_cb_ai_changed (Combobox* ctl, int value);

    void init_neighbours ();
    
protected:
    PlEditFrame ();

    void init_control (Control* par);

public:

    void preapare ();

    friend class PlEditFrameFactory;
};

class PlEditFrameFactory {
public:
    static PlEditFrame* create (Control* parent);
};

#endif	/* PLEDIT_FRAME_H */

