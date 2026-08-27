#ifndef H_LAND_HE_TAB_H
#define H_LAND_HE_TAB_H

/**
 * @file he_tab.h
 * @brief HE tab completion for component, API, and config paths
 */

int he_tab_begin( const char *word );
const char *he_tab_next( void );
void he_tab_end( void );

#endif
