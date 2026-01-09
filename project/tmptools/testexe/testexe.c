/*
 *    Description:  executable program template
 *         Author:  fpktools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

#include "skin/skin.h"

/* Usable macro 
gPLATFORM             String, the platform on which it is compiled, such as MTK platform is "MTK" (this macro is defined in the top Makefile of the SDK)
HARDWARE              String, compiled hardware (chip), MT7628 chip is "MT7628" (this macro is defined in the top Makefile of SDK)
gCUSTOM               String, the compiled product model, such as D218, is "D218" (this macro is defined in the top Makefile of the SDK) 
gPLATFORM__XXXX       Such as MTK platform will have gPLATFORM MTK macro definition
gHARDWARE__XXXX       Such as MT7628 chip hardware will have gHARDWARE__mt7628 macro definition
gCUSTOM__XXXX         Such as D218 products will have ggCUSTOM D218 macro definition
PROJECT_ID            String, is the project name
COM_ID                String, component name, Name of the directory where this component resides
COM_IDPATH            String, Full name of a component in the system, PROJECT_ID@COM_ID
EXE_ID                String, execute program name, Name of the directory where this execute program resides
EXE_IDPATH            String, Full name of a execute program name in the system, PROJECT_ID@EXE_ID
*/

/* Available skin interfaces (specific headers are in the top /doc/ API directory) 
link.h				implementation of general linker list
log.h	    		log call implementation
talk.h				implementation of common communication data types
param.h 			implementation of parameter structure and related functions
path.h				implementation of structure and related functions for object path and attribute path
utility.h   		miscellaneous function implementation
register.h			global register variable implementation
config.h			implementation function to get/set/list the config
project.h			provide unified project information operation interface for the system
com.h				implementation communication to other component function use talk structure or parameter structure
he2com.h			invokes the function implementation
serv.h		    	service call implementation
skinapi.h			define all the general component api
*/

/* Available Linux interfaces and macros
 * As normal programs under LINUX can call all Linux supported functions
 * If you want to use additional header files and libraries please give the location of header files and link libraries in mconfig under this directory
 * you can see the mconfig example for details of include additional header files and libraries
 */



/* Configure example key */
#define CFG_KEY_PROPERTY  "name"
/* Register example name */
#define REG_KEY_NAME      "loop"
/* Test component name */
#define TESTCOM_COM       PROJECT_ID"@testcom"
/* Component version - update this to verify deployment */
#define PROGRAM_VERSION   "v1.0.5 - 2025-01-07"

/* main function */
int main( int argc, const char **argv )
{
	int i;
	int *pi;
    talk_t cfg;
	obj_t objst;
    const char *ptr;

    /* Log version for debugging */
    app_info( "%s: process started [version: %s]", EXE_IDPATH, PROGRAM_VERSION );

	/* Make the object struct from TESTCOM_COM */
	objst = obj_create( TESTCOM_COM );
    /* Get the component configure */
	cfg = config_get( objst, NULL );
    /* Get the name attribute value */
	ptr = json_string( cfg, CFG_KEY_PROPERTY );

	/* make a register named "loop", initialized to 1, the register can be modified in real time by other components */
	i = 1;
	pi = reg_set_int( objst, REG_KEY_NAME, i );

    /* Loop times, print log every second */
	while( 1 )
    {
		// log
        app_info( "%s: this is the %d time print name vlaue %s", EXE_IDPATH, *pi, ptr );
		// register+1
		*pi = *pi+1;
		// sleep
        sleep( 3 );
    }
    app_fault( "%s: process completed [version: %s]", EXE_IDPATH, PROGRAM_VERSION );

	talk_free( cfg );
	obj_free( objst );
    return 0;
}



