#include <vector>

#include "engine/render.h"
#include "settings/team_infos.h"
#include "game/team.h"

#include "game/teams.h"

using namespace std;

vector<Team*> Teams::teams;

vector<plid_tu> Teams::orders;

Teams::Teams () {
}

void Teams::initialize (const std::vector<const TeamInfo*>& infos) {
    for (size_t ti = 0; ti < infos.size (); ti++) {
        const TeamInfo& tinfo = *infos[ti];
        teams.push_back (new Team (ti, tinfo));
    }
    orders.resize (teams.size ());

    teams.push_back (new Team (orders.size (), TeamInfos::at(0)));

    Render::load_teams (infos);
}

void Teams::uninitialize () {
    Render::free_teams ();

    for (size_t ti = 0; ti < teams.size (); ti++) {
        delete teams[ti];
    }
    teams.clear ();
    orders.clear ();
}

void Teams::update_score () {
    for (size_t ti = 0; ti < orders.size (); ti++) {
        orders[ti] = 0;
    }

    for (size_t oi = 0; oi < orders.size (); oi++) {
        for (size_t ti = 0; ti < orders.size (); ti++) {
            if ((*teams[oi]) > (*teams[ti])) {
                orders[oi]++;
            }
        }
    }

    for (size_t ti = 0; ti < orders.size (); ti++) {
        teams[ti]->order = orders[ti];
        teams[ti]->update_score ();
    }
}

void Teams::calc_stats () {
    for (size_t ti = 0; ti < orders.size (); ti++) {
        teams[ti]->calc_stats ();
    }
}

void Teams::draw_stat () {
    for (size_t ti = 0; ti < orders.size (); ti++) {
        teams[ti]->draw_stat ();
    }
}

