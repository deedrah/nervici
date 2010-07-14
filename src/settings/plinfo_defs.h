/* 
 * File:   plinfo_defs.h
 * Author: bobo
 *
 * Created on 8. červenec 2010, 17:31
 */

#ifndef PLINFO_DEFS_H
#define	PLINFO_DEFS_H

#include <glibmm/ustring.h>

using namespace Glib;

enum PlTypes {
    PT_Human, PT_AI
};

struct PlKeys {
    int left;
    int right;
    int jump;
};

struct AIInfo {
    int id;
};

struct PlInfo {
    int color;
    PlTypes type;
    ustring name;
    union {
        PlKeys keys;
        AIInfo ai;
    };
    ustring profil;
    int pitch;
};

#endif	/* PLINFO_DEFS_H */

