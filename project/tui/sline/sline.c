/*
 *    Description:  command component call depend on sline
 * 		Author:  dimmalex (dim), dimmalex@gmail.com
 * 	   Company:  ASHYELF
 */
#include "skin/skin.h"
#include <readline/readline.h>
#include <readline/history.h>



int he_command_execute( const char *cmd )
{
    int i;
	he_t h;
    talk_t ret;

	i = 0;
	switch( *cmd )
	{
		case '/':
			i = execute( 0, 1, cmd );
			if ( i != 0 )
			{
				perror("filename execute wrong");
			}
			break;
		case PRJ_COM_SEP:
			switch( *(cmd+1) )
			{
				case '\0':
					// @
					ret = project_list();
					talk_print( ret );
					talk_free( ret );
					break;
				case '?':
					// @?
					ret = com_project_list();
					talk_print( ret );
					talk_free( ret );
					break;
				case '@':
					// @@
					ret = com_register_list();
					talk_print( ret );
					talk_free( ret );
					break;
				case '*':
					// @*
					ret = com_list( NULL );
					talk_print( ret );
					talk_free( ret );
					break;
				default:
					// @land
					ret = com_list( cmd+1 );
					talk_print( ret );
					talk_free( ret );
					break;
			}
			break;
		case HE_CONFIG_TAG:
			// :
			// :land
			ret = config_list( cmd+1 );
			talk_print( ret );
			talk_free( ret );
			break;
		case OBJECT_API_GAPC:
			// .land@machine
			h = string2he( cmd+1 );
			if ( h != NULL )
			{
				if ( com_exist( h->o, h->m ) == true )
				{
					printf( "true\n" );
				}
				else
				{
					i = -1;
					printf( "false\n" );
				}
				he_free( h );
			}
			break;
		default:
			h = string2he( cmd );
			if ( h == NULL )
			{
				errno = EINVAL;
				perror("command parse wrong");
				i = -1;
				break;
			}
			/* exeucte the command */
			errno = 0;
			ret = he_execute( h );
			/* component return talk to print */
			if ( ret > tpanic )
			{
				talk_print( ret );
				talk_free( ret );
			}
			/* component return boole to print */
			else if ( ret == ttrue )
			{
				printf( ttrue_string"\n" );
			}
			else if ( ret == tfalse )
			{
				if ( errno == 0 )
				{
					printf( tfalse_string"\n" );
				}
				else
				{
					printf( tfalse_string", %s\n", strerror( errno ) );
				}
				i = -1;
			}
			/* component return error */
			else if ( ret == terror )
			{
				if ( errno == 0 )
				{
					printf( terror_string"\n" );
				}
				else
				{
					printf( terror_string", %s\n", strerror( errno ) );
				}
				i = -1;
			}
			/* call accident */
			else if ( ret == tpanic )
			{
				if ( errno == 0 )
				{
					printf( tpanic_string"\n" );
				}
				else
				{
					printf( tpanic_string", %s\n", strerror( errno ) );
				}
				i = -1;
			}
			he_free( h );
	}
	return i;
}

int main( int argc, const char **argv )
{
	char *line;
	struct stat st;

	if ( stat( "/etc/banner.he", &st ) == 0 )
	{
		shell( "cat /etc/banner.he" );
	}
	while (1)
	{
		line = readline("$ ");
		if ( line == NULL )
		{
			continue;
		}
		if ( isprint( *line ) == 0 || *line == '\0' || *line == '\r' || *line == '\n' )
		{
			free( line );
			continue;
		}
		if ( 0 == strncmp( line, "exit", 4 ) )
		{
			free( line );
			break;
		}
		else if ( 0 == strncmp( line, "ashy", 4 ) )
		{
			free( line );
			execl( "/bin/ash", "/bin/ash", "--login", NULL ); // open the new sessoin
			break;
		}
		else if ( 0 == strncmp( line, "set ", 4 ) )
		{
			talk_t v;
			talk_t cfg;
			char *gap;
			const char *attr;
			const char *value;
			const char *object;
			char prompt[NAME_MAX];

			object = line+4;
			if ( *object == '\0' )
			{
				free( line );
				continue;
			}
			cfg = sget( object, NULL );
			talk_print( cfg );
			add_history( line );
			snprintf( prompt, sizeof(prompt), "%s: ", object );
			while (1)
			{
				line = readline( prompt );
				if ( line == NULL )
				{
					continue;
				}
				if ( isprint( *line ) == 0 || *line == '\0' || *line == '\r' || *line == '\n' )
				{
					free( line );
					continue;
				}
				gap = strstr( line, "=" );
				if ( gap == NULL )
				{
					if ( 0 == strcmp( line, "e" ) )      // exit
					{
						break;
					}
					else if ( 0 == strcmp( line, "s" ) ) // set
					{
						if ( sset( object, cfg, NULL ) == true )
						{
							printf( ttrue_string"\n" );
						}
						else
						{
							printf( tfalse_string"\n" );
						}
						talk_free( cfg );
						break;
					}
					else if ( 0 == strcmp( line, "g" ) ) // get
					{
						talk_print( cfg );
					}
					else
					{
						v = attr_gets( cfg, line );
						if ( v != NULL )
						{
							talk_print( v );
						}
					}
				}
				else
				{
					attr = line;
					value = gap+1;
					*gap = '\0';
					if ( *value == '\0' )
					{
						value = NULL;
					}
					attr_sets_string( cfg, value, attr );
				}
				add_history( line );
				free( line );
			}
		}
		else if ( 0 == strncmp( line, "arp ", 4 )
			|| 0 == strncmp( line, "ping ", 5 )
			|| 0 == strncmp( line, "traceroute", 10 ) 
			|| 0 == strncmp( line, "ifconfig", 8 )
			|| 0 == strncmp( line, "route", 5 )
			|| 0 == strncmp( line, "netstat", 7 )
			|| 0 == strncmp( line, "iperf", 5 )
			|| 0 == strncmp( line, "tcpdump", 7 )
			|| 0 == strncmp( line, "mkdir ", 6 )
			|| 0 == strncmp( line, "telnet ", 7 )
			|| 0 == strncmp( line, "wg ", 3 )
			|| 0 == strncmp( line, "ip ", 3 )
			|| 0 == strncmp( line, "tip ", 4 )
			|| 0 == strncmp( line, "cd ", 3 )
			|| 0 == strncmp( line, "tftp ", 5 )
			|| 0 == strncmp( line, "curl ", 5 )
			)
		{
			add_history( line );
			shell( line );
		}
		else
		{
			add_history( line );
			he_command_execute( line );
		}
		free( line );
	}

	return 0;
}



