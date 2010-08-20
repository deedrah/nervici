/* 
 * File:   smyle_types.h
 * Author: bobo
 *
 * Created on 23. červenec 2010, 1:49
 */

#ifndef SMILE_TYPES_H
#define	SMILE_TYPES_H

enum SmileType {
    ST_pozi, ST_nega, ST_fleg, ST_iron, ST_cham, ST_ham, ST_count
};


#ifdef __cplusplus

inline SmileType operator ++ (SmileType& st, int) {
    st = (SmileType) (st + 1);
    return st;
}
#endif


#endif	/* SMYLE_TYPES_H */

