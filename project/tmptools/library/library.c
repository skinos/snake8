/*
 * ============================================================================
 * Skinos — shared library template (.a / .so)
 * ============================================================================
 *
 * Linked by components or executables via LIBA_ADDIN / LIBSO_ADDIN in mconfig.
 * Not a HE object by itself unless a component wraps calls into this library.
 *
 * Copy workflow
 *   1) Rename library.c / library.h (and this comment block).
 *   2) Update include guard in the .h file.
 *   3) Trim #include "skin/skin.h" if you do not use Skinos APIs here.
 * ============================================================================
 */

#include "skin/skin.h"
#include "library.h"

int library_api( int c, int i )
{
	(void)c;
	(void)i;
	return 0;
}
