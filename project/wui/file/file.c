#include "skin/skin.h"

/* Managed directory: /tmp/file on device, /tmp/skinos/file on slave */
#define FILE_DIR  "/tmp/file"



boole_t _setup( obj_t this, param_t param )
{
    struct stat st;
    char dir[PATH_MAX];

    snprintf( dir, sizeof(dir), "%s", FILE_DIR );
    if ( stat( dir, &st ) != 0 )
    {
        if ( mkdir( dir, REGULAR_DIR_MODE ) != 0 )
        {
            return tfalse;
        }
        return ttrue;
    }
    if ( !S_ISDIR( st.st_mode ) )
    {
        errno = ENOTDIR;
        return tfalse;
    }
    return ttrue;
}

/* delete[ name ] — delete a file under FILE_DIR by filename */
talk_t _delete( obj_t this, param_t param )
{
    struct stat st;
    size_t dlen;
    const char *name;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    name = param_string( param, 1 );
    if ( name == NULL || *name == '\0' || strchr( name, '/' ) != NULL )
    {
        errno = EINVAL;
        return tfalse;
    }
    if ( strcmp( name, "." ) == 0 || strcmp( name, ".." ) == 0 )
    {
        errno = EINVAL;
        return tfalse;
    }

    snprintf( dir, sizeof(dir), "%s", FILE_DIR );
    dlen = strlen( dir );
    snprintf( path, sizeof(path), "%s/%s", dir, name );

    if ( strncmp( path, dir, dlen ) != 0 || path[dlen] != '/' )
    {
        errno = EPERM;
        return tfalse;
    }
    if ( stat( path, &st ) != 0 )
    {
        return tfalse;
    }
    if ( !S_ISREG( st.st_mode ) )
    {
        errno = EISDIR;
        return tfalse;
    }
    if ( unlink( path ) != 0 )
    {
        return tfalse;
    }
    return ttrue;
}

/* list[] — list files under FILE_DIR as { "name": { "path", "size" } } */
talk_t _list( obj_t this, param_t param )
{
    DIR *pdir;
    struct stat st;
    struct dirent *pent;
    talk_t ret;
    talk_t item;
    int size;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    snprintf( dir, sizeof(dir), "%s", FILE_DIR );
    if ( stat( dir, &st ) != 0 )
    {
        if ( mkdir( dir, REGULAR_DIR_MODE ) != 0 )
        {
            return NULL;
        }
    }
    else if ( !S_ISDIR( st.st_mode ) )
    {
        errno = ENOTDIR;
        return NULL;
    }

    ret = json_create( NULL );
    if ( ret == NULL )
    {
        return NULL;
    }

    pdir = opendir( dir );
    if ( pdir == NULL )
    {
        return ret;
    }
    while ( ( pent = readdir( pdir ) ) != NULL )
    {
        if ( pent->d_name[0] == '.' || pent->d_name[0] == '\0' )
        {
            continue;
        }
        snprintf( path, sizeof(path), "%s/%s", dir, pent->d_name );
        if ( stat( path, &st ) != 0 || !S_ISREG( st.st_mode ) )
        {
            continue;
        }
        item = json_create( NULL );
        if ( item == NULL )
        {
            continue;
        }
        if ( st.st_size > INT_MAX )
        {
            size = INT_MAX;
        }
        else
        {
            size = (int)st.st_size;
        }
        json_set_string( item, "path", path );
        json_set_number( item, "size", size );
        json_set_value( ret, pent->d_name, item );
    }
    closedir( pdir );

    return ret;
}
