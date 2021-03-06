#include "app.h"
#include "gui/label_button.h"
#include "frames/game_frame.h"
#include "frames/pledit_frame.h"
#include "frames/options_frame.h"

#include "frames/start_frame.h"

#define START_WIDTH 300
#define START_HEIGHT (40 * 4 + 5 * 8)

static const ControlParameters frame_parms = {
    (1024 - START_WIDTH) / 2,
    (768 - START_HEIGHT) / 2,
    START_WIDTH,
    START_HEIGHT,
    30};

static const ControlParameters btn_new_game_parms = {
    8.0,
    8.0,
    frame_parms.w - 16,
    40.0,
    frame_parms.font_size};

static const ControlParameters btn_pledit_parms = {
    8.0,
    btn_new_game_parms.y + btn_new_game_parms.h + 8,
    btn_new_game_parms.w,
    btn_new_game_parms.h,
    btn_new_game_parms.font_size};

static const ControlParameters btn_options_parms = {
    8.0,
    btn_pledit_parms.y + btn_pledit_parms.h + 8,
    btn_new_game_parms.w,
    btn_new_game_parms.h,
    btn_new_game_parms.font_size};

static const ControlParameters btn_quit_parms = {
    8,
    btn_options_parms.y + btn_options_parms.h + 8,
    btn_new_game_parms.w,
    btn_new_game_parms.h,
    btn_new_game_parms.font_size};

StartFrame::StartFrame () :
    Frame (frame_parms) {
}

void StartFrame::init_control (Control* par) {
    Frame::init_control (par);

    LabelButton* btn_new_game = LabelButtonFactory::create (this,
        _("new game"),
        btn_new_game_parms,
        "btn_new_game");

    btn_new_game->register_on_clicked (OnClicked (this,
        &StartFrame::btn_new_game_clicked));

    LabelButton* btn_pledit = LabelButtonFactory::create (this,
        _("players"),
        btn_pledit_parms,
        "btn_pledit");
    btn_pledit->register_on_clicked (OnClicked (this,
        &StartFrame::btn_pledit_clicked));

    LabelButton* btn_options = LabelButtonFactory::create (this,
        _("options"),
        btn_options_parms,
        "btn_options");
    btn_options->register_on_clicked (OnClicked (this,
        &StartFrame::btn_options_clicked));

    LabelButton* btn_quit = LabelButtonFactory::create (this,
        _("quit"),
        btn_quit_parms,
        "btn_quit");
    btn_quit->register_on_clicked (OnClicked (this,
        &StartFrame::btn_quit_clicked));

    btn_new_game->set_neighbours (btn_quit, NULL, NULL, btn_pledit);
    btn_pledit->set_neighbours (btn_new_game, NULL, NULL, btn_options);
    btn_options->set_neighbours (btn_pledit, NULL, NULL, btn_quit);
    btn_quit->set_neighbours (btn_options, NULL, NULL, btn_new_game);
}

void StartFrame::btn_new_game_clicked (Control* ctl) {
    app.switch_to_game_frame ();
    app.get_game_frame ().preapare ();
}

void StartFrame::btn_pledit_clicked (Control* ctl) {
    app.switch_to_pledit_frame ();
    app.get_pledit_frame ().preapare ();
}

void StartFrame::btn_quit_clicked (Control* ctl) {
    app.quit ();
}

void StartFrame::btn_options_clicked (Control* ctl) {
    app.switch_to_options_frame ();
    app.get_options_frame ().prepare ();
}

StartFrame* StartFrameFactory::create (Control* par) {
    StartFrame* result = new StartFrame ();
    result->set_name ("start_frame");
    result->init_control (par);
    result->show_all ();
    return result;
}

