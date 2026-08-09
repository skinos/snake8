#ifndef H_LAND_MXTALK_PRIV_H
#define H_LAND_MXTALK_PRIV_H

/**
 * @file mxtalk_priv.h
 * @brief Private create helper for m1talk.c / m2talk.c (not for app use)
 */

#include "mxtalk.h"

#ifdef __cplusplus
extern "C" {
#endif

mxtalk_t mx_create_map( const char *path, int depth, int max_l1, int max_l2,
	int max_l2_pool, int name_max, int max_heap );

#ifdef __cplusplus
}
#endif

#endif
