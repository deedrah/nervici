/* 
 * File:   teaminfo.h
 * Author: bobo
 *
 * Created on 21. červenec 2010, 20:39
 */

#ifndef TEAMINFO_H
#define	TEAMINFO_H

#include <glibmm/ustring.h>

using namespace Glib;

struct TeamInfo {
    u_int32_t color;
    ustring name;
};



#endif	/* TEAMINFO_H */

