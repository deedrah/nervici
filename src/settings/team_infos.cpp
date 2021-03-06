#include <glibmm/ustring.h>

#include "basic_defs.h"
#include "utils.h"
#include "logger.h"
#include "settings/setting.h"
#include "settings/settings.h"
#include "settings/team_info.h"

#include "settings/team_infos.h"

using namespace Glib;

const static TeamInfo def_infos[TEAMS_COUNT] = {
    {
        0,
        0x000000,
        ""},
    {
        1,
        0xff8080,
        "team 17"},
    {
        2,
        0xff80ff,
        "team 24"},
    {
        3,
        0x80ff80,
        "team 39"},
    {
        4,
        0x8080ff,
        "team 18+"}};

TeamInfos TeamInfos::instance;

TeamInfos& team_infos = TeamInfos::get_instance ();

TeamInfos::TeamInfos () {
}

TeamInfos::~TeamInfos () {
    logger.fineln ("freeing teams setting");
    for (size_t ti = 1; ti < TEAMS_COUNT; ti++) {
        delete infos[ti];
        infos[ti] = NULL;
    }
}

void TeamInfos::load () {
    logger.fineln ("loading teams setting");

    Setting& set = settings.teams ();

    ustring section;

    infos[0] = new TeamInfo (def_infos[0]);

    for (size_t ti = 1; ti < TEAMS_COUNT; ti++) {
        section = "team";
        section+= uint_to_string (ti);

        infos[ti] = new TeamInfo ();
        infos[ti]->id = ti;
        infos[ti]->color = set.read_hex (section, "color", def_infos[ti].color);
        infos[ti]->name = set.read_string (section, "name", def_infos[ti].name);
    }
}

void TeamInfos::save () {
    logger.fineln ("saving teams setting");

    Setting& set = settings.teams ();

    ustring section;

    for (size_t ti = 1; ti < TEAMS_COUNT; ti++) {
        section = "team";
        section+= uint_to_string (ti);

        set.write_hex (section, "color", infos[ti]->color);
        set.write_string (section, "name", infos[ti]->name);
    }
}
