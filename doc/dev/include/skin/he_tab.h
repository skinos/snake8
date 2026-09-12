#ifndef H_LAND_HE_TAB_H
#define H_LAND_HE_TAB_H

/**
 * @file he_tab.h
 * @brief HE tab completion for component, API, config, and project names
 *
 * Leading `.` / `?` keep component+API completion; leading `:` / `@`
 * complete project names via project_list (same as `:land` / `@land`).
 */

int he_tab_begin( const char *word );
const char *he_tab_next( void );
void he_tab_end( void );

#endif
