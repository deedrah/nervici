/*
 * gen0.cpp
 *
 *  Created on: 24.8.2010
 *      Author: bobo
 */

#include <iostream>
#include <cmath>

#include "main.h"
#include "basic_defs.h"
#include "point.h"
#include "game/world.h"
#include "game/death_cause.h"
#include "engine/render.h"

#include "ais/gen0.h"

using namespace std;

Index::Index (size_t v) :
    value (v % MAX_STEPS) {

}

inline Index::operator size_t () const {
    return value;
}

inline Index& Index::operator ++ (int) {
    value++;
    value %= MAX_STEPS;
    return *this;
}

AIGen0::AIGen0 (plid_tu nid) :
    id (nid) {
    plan.resize (MAX_STEPS, KS_None);
    target.x = random () % World::get_width ();
    target.y = random () % World::get_height ();
}

KeySt AIGen0::get_next_step () {
    return plan[0];
}

void AIGen0::calc (const FPoint& pos, int angle, int jump_time, plsize_tu head) {
    //cout << "id: " << int(id) << '\n';
    barier = 0;
    if (target_distance (pos) < 5) {
        target.x = random () % World::get_width ();
        target.y = random () % World::get_height ();
    }
    //Render::draw_fake_face(target);

    double dist = target_distance (pos);
    size_t len = make_plan (pos, angle, jump_time, head, 0, KS_None, 0, dist);
    if ((len <= 4 && plan[0] == KS_None) || len == 0) {
        plan[0] = KS_Jump;
    }
    /*cout.precision(10);
    cout << dist << " " << int (plan[0]) <<'\n';*/
}

size_t AIGen0::make_plan (const FPoint& prev_pos, int prev_angle, int jumptime,
        plsize_tu head, size_t distance, KeySt def, size_t max, double& dist) {
    FPoint pos;
    size_t dis_none;
    size_t dis_left;
    size_t dis_right;
    double target_none;
    double target_left;
    double target_right;
    int next_jump;
    int angle = prev_angle;

    if (distance == MAX_STEPS) {
        return MAX_STEPS;
    }

    barier++;
    if (distance > 4 && barier > 5000) {
        return distance;
    }

    next_jump = (jumptime > 0) ? jumptime - 1 : 0;

    if (def == KS_Left && distance <= max) {
        angle = (prev_angle + angles - 1) % angles;
        pos.x = prev_pos.x + icos[angle] / 2;
        pos.y = prev_pos.y + isin[angle] / 2;
        target_left = target_distance (pos);
        dist = (dist <= target_left) ? dist : target_left;
        if (will_survive (pos, jumptime, head)) {
            dis_left = make_plan (pos, angle, next_jump, (head + 1), distance
                    + 1, KS_Left, max, target_left);

            dist = (dist <= target_left) ? dist : target_left;
            plan[distance] = KS_Left;
            return dis_left;
        }
        return distance;
    }

    if (def == KS_Right && distance <= max) {
        angle = (prev_angle + 1) % angles;
        pos.x = prev_pos.x + icos[angle] / 2;
        pos.y = prev_pos.y + isin[angle] / 2;
        target_right = target_distance (pos);
        dist = (dist <= target_right) ? dist : target_right;
        if (will_survive (pos, jumptime, head)) {
            dis_right = make_plan (pos, angle, next_jump, (head + 1), distance
                    + 1, KS_Right, max, target_right);

            dist = (dist <= target_right) ? dist : target_right;
            plan[distance] = KS_Right;
            return dis_right;
        }
        return distance;
    }

    pos.x = prev_pos.x + icos[angle] / 2;
    pos.y = prev_pos.y + isin[angle] / 2;
    target_none = target_distance (pos);
    if (will_survive (pos, jumptime, head)) {
        dis_none = make_plan (pos, angle, next_jump, (head + 1), distance + 1,
                KS_None, distance + 1, target_none);
        if (distance > 0) {
            if (dis_none == MAX_STEPS) {
                plan[distance] = KS_None;
                dist = (dist <= target_none) ? dist : target_none;
                return MAX_STEPS;
            }

            size_t delta = dis_none - distance;
            if (delta < 4 || (delta > 36 && delta < 44) || (delta > 76 && delta
                    < 80)) {
                dist = (dist <= target_none) ? dist : target_none;
                return dis_none;
            }
            if (delta > 80) {
                dis_none = distance + 80;
            }
        }
    } else {
        dis_none = distance;
    }

    angle = (prev_angle + angles - 1) % angles;
    pos.x = prev_pos.x + icos[angle] / 2;
    pos.y = prev_pos.y + isin[angle] / 2;
    target_left = target_distance (pos);
    if (will_survive (pos, jumptime, head)) {
        dis_left = make_plan (pos, angle, next_jump, (head + 1), distance + 1,
                KS_Left, (distance > 0) ? dis_none : distance + 1, target_left);

        if (dis_left == MAX_STEPS && distance > 0) {
            plan[distance] = KS_Left;
            dist = (dist <= target_left) ? dist : target_left;
            return MAX_STEPS;
        }
    } else {
        dis_left = distance;
    }

    angle = (prev_angle + 1) % angles;
    pos.x = prev_pos.x + icos[angle] / 2;
    pos.y = prev_pos.y + isin[angle] / 2;
    target_right = target_distance (pos);
    if (will_survive (pos, jumptime, head)) {
        dis_right = make_plan (pos, angle, next_jump, (head + 1), distance + 1,
                KS_Right, (distance > 0) ? dis_none : distance + 1, target_right);

        if (dis_right == MAX_STEPS && distance > 0) {
            plan[distance] = KS_Right;
            dist = (dist <= target_right) ? dist : target_right;
            return MAX_STEPS;
        }
    } else {
        dis_right = distance;
    }

    /*if (jumptime == 0) {
     jumptime = JUMP_REPEAT;
     next_jump = jumptime - 1;
     angle = prev_angle;
     pos.x = prev_pos.x + icos[angle] / 2;
     pos.y = prev_pos.y + isin[angle] / 2;
     if (will_survive (pos, jumptime, head)) {
     dis_jump = make_plan (pos, angle, next_jump, (head + 1), distance
     + 1, KS_None, distance + 1);

     if (dis_jump == MAX_STEPS) {
     plan[distance] = KS_Jump;
     return MAX_STEPS;
     }

     } else {
     dis_jump = distance;
     }
     }*/

    /* if (distance == 0) {
     cout << distance << " : " << dis_none << " < " << dis_left << " > "
     << dis_right << '\n';
     }*/

    if (distance > 0) {
        if (dis_none >= dis_left && dis_none >= dis_right) {
            plan[distance] = KS_None;
            dist = (dist <= target_none) ? dist : target_none;
        } else if (dis_left >= dis_none && dis_left >= dis_right) {
            plan[distance] = KS_Left;
            dist = (dist <= target_left) ? dist : target_left;
        } else {
            plan[distance] = KS_Right;
            dist = (dist <= target_right) ? dist : target_right;
        }
    } else {
        if (target_none <= target_left && target_none <= target_right) {
            plan[distance] = KS_None;
            dist = (dist <= target_none) ? dist : target_none;
        } else if (target_left <= target_none && target_left <= target_right) {
            plan[distance] = KS_Left;
            dist = (dist <= target_left) ? dist : target_left;
        } else {
            plan[distance] = KS_Right;
            dist = (dist <= target_right) ? dist : target_right;
        }
    }
    return dis_none;
}

bool AIGen0::will_survive (const FPoint& pos, int jumptime, plsize_tu head) {
    Point ipos;
    DeathCause cause;

    ipos.x = pos.x - 1;
    ipos.y = pos.y - 1;
    fields.calc (pos);
    //Render::draw_fake_face(ipos);
    if (jumptime <= JUMP_REPEAT - JUMP_LENGTH) {
        return World::will_survive (ipos, fields, id, head, cause);
    } else {
        return World::simple_will_survive (ipos, fields);
    }
}

double AIGen0::target_distance (const FPoint& pos) {
    double delta_x = pos.x - target.x;
    double delta_y = pos.y - target.y;
    return sqrt (delta_x * delta_x + delta_y * delta_y);
}
