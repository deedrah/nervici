#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mods/nervici.h"
using namespace Nervici;

#include "mods/mod_interface.h"

static const char * const exts[] = {
    NULL
};

static const struct ModInfo info = {
    "cervici",
    "bobo",
    "proste cervici",
    {16,
        1, 1, 1, 0, 1, 1, 1,
        12, 10, 0, 0, 1, 1000, 0,
        {
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
        },
        {
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
        }}
};


#define WAIT_TIME 800

class Cervici : public ModInterface {
private:
    ModRunnerInfo runner_info;
    GameSetting set;

    void begin_start_procedure () {
        int sid;

        set_semafor (SEMAFOR_R1);
        game_wait (WAIT_TIME);

        for (int pi = 0; pi < set.playersCount; pi++) {
            if (is_pl_human (pi)) {
                sid = world_find_free_start ();
                if (sid < set.startsCount) {
                    give_pl_start (pi, sid);
                }
            }
        }
        set_semafor (SEMAFOR_R1 | SEMAFOR_R2);
        game_wait (WAIT_TIME);


        for (int pi = 0; pi < set.playersCount; pi++) {
            if (!is_pl_human (pi)) {
                sid = world_find_free_start ();
                if (sid < set.startsCount) {
                    give_pl_start (pi, sid);
                }
            }
        }
        set_semafor (SEMAFOR_R1 | SEMAFOR_R2 | SEMAFOR_R3);
        game_wait (WAIT_TIME);

        play_music (0);
        for (int pi = 0; pi < set.playersCount; pi++) {
            start_pl (pi);
        }
        set_semafor (SEMAFOR_G1);
    }

public:

    Cervici () {
        int ei = 0;
        const char* ext = exts[ei];

        while (ext != NULL) {
            runner_info.extensions.push_back (ext);
            ei++;
            ext = exts[ei];
        }
    }

    const ModRunnerInfo & get_runner_info () {
        return runner_info;
    }

    const ModInfo * get_info (const Glib::ustring & script) {
        return &info;
    }

    void on_game_start (const GameSetting & nset) {
        set = nset;

        begin_start_procedure ();
    }

    void on_death (int plid) {
        revive_pl (plid);
    }

    void after_step () {
        int p;

        for (p = 0; p < set.playersCount; p++) {
            if (is_pl_live (p)) {
                inc_pl_score (p, 1);
            }
        }

        /*if (live_pls_count () <= 0) {
            stop_music ();
            set_semafor (SEMAFOR_OF);

            if (get_round () == set->rounds) {
                end_game ();
            } else {
                wait_for_space ();

                clear_playerground ();
                next_round ();
                begin_start_procedure ();
            }
        }*/
    }

    void on_pozi_smile (plid_tu plid, int lvl) {
        score_ti iron = get_pl_ironzie (plid);

        if (iron <= 0) {
            inc_pl_score (plid, set.bonus * lvl);
        } else {
            dec_pl_score (plid, set.bonus * lvl * iron);
            set_pl_ironize (plid, 0);
        }
    }

    void on_nega_smile (plid_tu plid, int lvl) {
        score_ti iron = get_pl_ironzie (plid);

        if (iron <= 0) {
            dec_pl_score (plid, set.bonus * lvl);
        } else {
            inc_pl_score (plid, set.bonus * lvl + iron);
            set_pl_ironize (plid, 0);
        }
    }

    void on_iron_smile (plid_tu plid, int lvl) {
        inc_pl_ironize (plid, lvl);
    }
};

ModInterface* get_face () {
    return new Cervici ();
}

