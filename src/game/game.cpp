#include <SDL.h>
#include <cmath>

#include "logger.h"
#include "engine/render.h"
#include "engine/audio.h"
#include "settings/team_info.h"
#include "mods/mod_interface.h"
#include "mods/mods.h"
#include "game/statistic.h"
#include "game/world.h"
#include "game/smiles.h"
#include "game/smile.h"
#include "game/player.h"
#include "game/players.h"
#include "game/team.h"
#include "game/teams.h"
#include "game/game_info.h"

#include "game/game.h"

using namespace std;

class GameImpl: public Game {
private:
    GameSetting set;

    SmileSetting smile_set;

    bool end;

    bool abort;

    timer_ti timer;

    Uint32 sdl_time;

    size_t sl_count;

    size_t nosl_count;

    size_t nosmlsl_count;

    static GameImpl instance;

    GameImpl ();

    void sleep (timer_ti pause);

    void run_statistic ();

public:

    bool initialize (const GameInfo & info);

    void run ();

    void uninitialize ();

    void clear_playerground ();

    void clear_status ();

    void end_game ();

    timer_ti get_speed ();

    void next_round ();

    round_tu get_round ();

    void set_speed (timer_ti value);

    void set_timer (timer_ti value);

    void wait (timer_ti time);

    void wait_for_space ();

    static GameImpl& get_instance ();
};

GameImpl GameImpl::instance;

Game& game = GameImpl::get_instance ();

GameImpl::GameImpl () :
    sl_count (0), nosl_count (0), nosmlsl_count (0) {

}

bool GameImpl::initialize (const GameInfo& info) {
    logger.fineln ("initialize game");

    set = info.setting;
    set_speed (info.setting.speed);
    smile_set = info.smiles;

    render.draw_game_screen ();

    if (!world.initialize ())
        return false;

    if (!teams.initialize (info.team_infos))
        return false;

    if (!players.initialize (info.pl_infos,
        info.pl_teams,
        info.setting.max_length))
        return false;

    if (!smiles.initialize (info.smiles))
        return false;

    end = false;
    abort = false;
    round = 1;
    timer = 0;

    render.draw_semafor (SEMAFOR_OFF);
    render.draw_round (round);

    clear_playerground ();

    mods.load_mod (info.mod_id);
    return true;
}

void GameImpl::uninitialize () {
    logger.fineln ("uninitialize game");

    mods.unload_mod ();

    smiles.uninitialize ();
    players.uninitialize ();
    teams.uninitialize ();
    world.uninitialize ();
}

void GameImpl::sleep (timer_ti pause) {
    sdl_time += pause;
    timer_ti delta = sdl_time - SDL_GetTicks ();
    if (delta > 0) {
        sl_count++;
        SDL_Delay (delta);
    } else if (delta < -pause) {
        sdl_time = SDL_GetTicks () - pause;
        nosmlsl_count++;
    } else {
        nosl_count++;
    }
}

static int compare_stat (const Statistic& st1,
        const Statistic& st2,
        StatColumn col) {
    int sub;

    switch (col) {
    case STC_score:
        return st1.score - st2.score;
    case STC_length:
        return st1.steps - st2.steps;
    case STC_deaths:
        return st1.deaths - st2.deaths;
    case STC_crashes:
        return st1.crashes - st2.crashes;
    case STC_kills:
        return st1.kills - st2.kills;
    case STC_killed:
        return st1.killed - st2.killed;
    case STC_selfs:
        return st1.selfs - st2.selfs;
    case STC_jumps:
        return st1.jump - st2.jump;
    case STC_pozi:
        sub = render.get_column_sub (col);
        return st1.smiles[ST_pozi][sub] - st2.smiles[ST_pozi][sub];
    case STC_nega:
        sub = render.get_column_sub (col);
        return st1.smiles[ST_nega][sub] - st2.smiles[ST_nega][sub];
    case STC_fleg:
        sub = render.get_column_sub (col);
        return st1.smiles[ST_fleg][sub] - st2.smiles[ST_fleg][sub];
    case STC_iron:
        sub = render.get_column_sub (col);
        return st1.smiles[ST_iron][sub] - st2.smiles[ST_iron][sub];
    case STC_cham:
        sub = render.get_column_sub (col);
        return st1.smiles[ST_cham][sub] - st2.smiles[ST_cham][sub];
    case STC_ham:
        sub = render.get_column_sub (col);
        return st1.smiles[ST_ham][sub] - st2.smiles[ST_ham][sub];
    default:
        return 0;
    }
}

static int compare_pl (const Player& pl1, const Player& pl2, StatColumn col) {
    switch (col) {
    case STC_name:
        return pl1.get_name ().compare (pl2.get_name ());
    default:
        return compare_stat (pl1.stat, pl2.stat, col);
    }
}

static int compare_team (const Team& te1, const Team& te2, StatColumn col) {
    switch (col) {
    case STC_name:
        return te1.info.name.compare (te2.info.name);
    default:
        return compare_stat (te1.stat, te2.stat, col);
    }
}

void GameImpl::run_statistic () {
    render.reset_columns_sub ();
    render.draw_stat_screen ();
    SDL_Event event;

    StatColumn order_col = STC_score;

    audio.music_play (MT_Stat);
    players.calc_stats ();
    teams.calc_stats ();
    players.draw_stat ();
    teams.draw_stat ();

    vector<plid_tu> pl_orders;
    pl_orders.resize (players.count ());
    vector<plid_tu> team_orders;
    team_orders.resize (teams.count ());

    while (!abort) {
        while (SDL_PollEvent (&event)) {
            switch (event.type) {
            case SDL_QUIT:
                abort = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_SPACE:
                    abort = true;
                    break;
                default:
                    break;
                }
            case SDL_MOUSEBUTTONDOWN: {
                StatColumn col = render.get_column_from_pos (event.button.x,
                    event.button.y);
                if (col != STC_count) {
                    switch (event.button.button) {
                    case SDL_BUTTON_RIGHT:
                        render.cycle_column_sub (col);
                        order_col = STC_count;
                    case SDL_BUTTON_LEFT:
                        if (order_col != col) {
                            order_col = col;

                            for (plid_tu plid = 0; plid < players.count (); plid++) {
                                const Player& pl = players[plid];
                                pl_orders[plid] = 0;
                                for (plid_tu otherid = 0; otherid
                                        < players.count (); otherid++) {
                                    const Player& other = players[otherid];
                                    int cmp = compare_pl (pl, other, order_col);
                                    if (cmp < 0) {
                                        pl_orders[plid]++;
                                    } else if (cmp == 0) {
                                        if (pl.order > other.order) {
                                            pl_orders[plid]++;
                                        }
                                    }
                                }
                            }
                            for (plid_tu plid = 0; plid < players.count (); plid++) {
                                players[plid].order = pl_orders[plid];
                            }

                            for (plid_tu tid = 0; tid < teams.count (); tid++) {
                                team_orders[tid] = 0;
                                const Team& te = teams[tid];
                                plid_tu old_order = te.order;
                                for (plid_tu otheri = 0; otheri
                                        < teams.count (); otheri++) {
                                    const Team& other = teams[otheri];
                                    int cmp = compare_team (te,
                                        other,
                                        order_col);
                                    if (cmp < 0) {
                                        team_orders[tid]++;
                                    } else if (cmp == 0) {
                                        if (old_order > other.order) {
                                            team_orders[tid]++;
                                        }
                                    }
                                }
                            }
                            for (plid_tu tid = 0; tid < teams.count (); tid++) {
                                teams[tid].order = team_orders[tid];
                            }
                        } else {
                            for (plid_tu plid = 0; plid < players.count (); plid++) {
                                Player& pl = players[plid];
                                pl.order = (players.count () - pl.order - 1);
                            }
                            for (plid_tu tid = 0; tid < teams.count (); tid++) {
                                Team& te = teams[tid];
                                te.order = teams.count () - te.order - 1;
                            }
                        }
                        players.draw_stat ();
                        teams.draw_stat ();

                        break;
                    }
                }
                break;
            }
            default:
                break;
            }
        }
        audio.music_update ();
        SDL_Delay (10);
    }
}

void GameImpl::run () {
    SDL_Event event;
    size_t steps = 0;
    uint8_t *keys;

    logger.fineln ("entering game loop");

    //clock_gettime (CLOCK_REALTIME, &time);
    sdl_time = SDL_GetTicks ();
    players.update_score ();
    render.draw_timer (timer);
    mods.face ().on_game_start (set, smile_set);

    while (!end && !abort) {
        while (SDL_PollEvent (&event)) {
            switch (event.type) {
            case SDL_QUIT:
                abort = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    abort = true;
                    break;
                default:
                    break;
                }
                break;
            }
        }

        keys = SDL_GetKeyState (NULL);

        mods.face ().before_step ();
        players.finish_step_preparation ();
        players.step (keys);
        smiles.step ();
        world.check_starts ();
        mods.face ().after_step ();

        //off
        /*mods.face ().before_step ();
         players.step (keys);
         smiles.step ();
         world.check_starts ();
         mods.face ().after_step ();*/

        steps += speed;
        if (steps > 10) {
            world.render_changed_items ();
            players.update_bodies ();
            players.update_score ();
            teams.update_score ();
            steps = 0;
        }
        //render.update_screen ();

        audio.music_update ();

        sleep (speed);

        bool sub_zero = timer < 0;
        timer += speed;
        render.draw_timer (timer);

        if (sub_zero) {
            if (timer >= 0) {
                timer = 0;
                render.draw_timer (timer);
                mods.face ().on_timer ();
            }
        }
        players.timer (speed);
        //steps++;
    }

    audio.music_stop ();
    audio.music_set_rate (1);

    if (!abort) {
        render.draw_end ();
        wait_for_space ();
    }
    if (!abort) {
        run_statistic ();
    }

    mods.face ().on_game_end ();
}

void GameImpl::set_speed (timer_ti value) {
    speed = value;
    if (speed < BASE_SPEED / 2)
        speed = BASE_SPEED / 2;
    if (speed > BASE_SPEED * 2)
        speed = BASE_SPEED * 2;
    audio.music_set_rate ((BASE_SPEED * 1.0) / speed);
}

void GameImpl::wait (timer_ti time) {
    SDL_Event event;
    int rest = time;

    world.render_changed_items ();
    players.update_bodies ();
    players.update_score ();
    teams.update_score ();

    while (!end && !abort && rest > 0) {

        while (SDL_PollEvent (&event)) {
            switch (event.type) {
            case SDL_QUIT:
                abort = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    abort = true;
                }
                break;
            }
        }
        rest -= 10;
        sleep (10);
    }
}

void GameImpl::wait_for_space () {
    SDL_Event event;

    world.render_changed_items ();
    players.update_bodies ();
    players.update_score ();
    teams.update_score ();

    while (!abort) {
        while (SDL_PollEvent (&event)) {
            switch (event.type) {
            case SDL_QUIT:
                abort = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    abort = 1;
                    break;
                case SDLK_SPACE:
                    return;
                default:
                    break;
                }
                break;
            }
        }
        sleep (10);
    }
}

void GameImpl::next_round () {
    round++;
    render.draw_round (round);
}

void GameImpl::clear_playerground () {
    world.clear ();
    render.clear_playerground ();
    players.erase ();
    smiles.erase ();
}

void GameImpl::end_game () {
    end = true;
}

void GameImpl::set_timer (timer_ti value) {
    timer = value;
    render.draw_timer (timer);
}

void GameImpl::clear_status () {

}

inline GameImpl& GameImpl::get_instance () {
    return instance;
}
