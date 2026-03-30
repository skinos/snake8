#ifndef TMPTOOLS_LIBRARY_TEMPLATE_H
#define TMPTOOLS_LIBRARY_TEMPLATE_H
/*
 * ============================================================================
 * Skinos — shared library public header (rename guard + symbols when copying)
 * ============================================================================
 *
 * Put only stable, exported symbols here.  Implementation lives in library.c.
 * If this library is pure C/POSIX, you may drop skin/skin.h from the .c file
 * and keep this header free of Skinos types.
 * ============================================================================
 */

/**
 * Placeholder API — replace with real functions.
 *
 * @param c  example parameter
 * @param i  example parameter
 * @return   convention: 0 success, non-zero error (define your own codes)
 */
int library_api( int c, int i );

#endif /* TMPTOOLS_LIBRARY_TEMPLATE_H */
