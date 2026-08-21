#include "skin/skin.h"



boole_t _setup( obj_t this, param_t param )
{
    DIR *pdir;
    struct stat st;
    struct dirent *pent;
    char dir[PATH_MAX];
    char path[PATH_MAX];
    char object[NAME_MAX];

    internal2path( dir, sizeof(dir), NULL );
    pdir = opendir( dir );
    if ( pdir == NULL )
    {
        return ttrue;
    }
    while ( ( pent = readdir( pdir ) ) != NULL )
    {
        if ( pent->d_name[0] == '.' || pent->d_name[0] == '\0' )
        {
            continue;
        }
        internal2path( path, sizeof(path), "%s", pent->d_name );
        if ( stat( path, &st ) != 0 || !S_ISREG( st.st_mode ) || !( st.st_mode & S_IXUSR ) )
        {
            continue;
        }
        com_name2com( SCRIPT_PROJECT, pent->d_name, object, sizeof(object) );
        com_register( object, path, COM_FILE_EXECUTE );
    }
    closedir( pdir );

    return ttrue;
}

/* add[ name ] — create a shell component under internal dir */
talk_t _add( obj_t this, param_t param )
{
    int err;
    struct stat st;
    const char *name;
    char path[PATH_MAX];
    char object[NAME_MAX];
    char template[PATH_MAX];

    name = param_string( param, 1 );
    if ( name == NULL || *name == '\0' || strchr( name, '/' ) != NULL )
    {
        errno = EINVAL;
        return tfalse;
    }
    internal2path( path, sizeof(path), "%s", name );
    if ( stat ( path, &st ) == 0 )
    {
        errno = EEXIST;
        return tfalse;
    }

    misc2path( template, sizeof(template), "comshell" );
    if ( shell( "cp %s %s", template, path ) != 0 )
    {
        return tfalse;
    }
    if ( chmod( path, 0755 ) != 0 )
    {
        err = errno;
        unlink( path );
        errno = err;
        return tfalse;
    }
    
    com_name2com( SCRIPT_PROJECT, name, object, sizeof(object) );
    if ( com_register( object, path, COM_FILE_EXECUTE ) == false )
    {
        err = errno;
        unlink( path );
        errno = err;
        return tfalse;
    }
    return ttrue;
}

/* delete[ object ] — unregister and delete a shell component */
talk_t _delete( obj_t this, param_t param )
{
    char type;
    size_t dlen;
    const char *object;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    object = param_string( param, 1 );
    if ( object == NULL || *object == '\0' )
    {
        errno = EINVAL;
        return tfalse;
    }

    type = com_path( object, path, sizeof(path) );
    if ( type != COM_FILE_EXECUTE )
    {
        if ( type == COM_FILE_LIB )
        {
            errno = EPERM;
        }
        return tfalse;
    }

    internal2path( dir, sizeof(dir), NULL );
    dlen = strlen( dir );
    if ( dlen > 0 && dir[dlen - 1] == '/' )
    {
        dlen--;
    }
    if ( strncmp( path, dir, dlen ) != 0 || path[dlen] != '/' )
    {
        errno = EPERM;
        return tfalse;
    }

    com_unregister( object );
    unlink( path );
    return ttrue;
}

/* view[ object ] — return base64-encoded content of any COM_FILE_EXECUTE object */
talk_t _view( obj_t this, param_t param )
{
    talk_t v;
    FILE *fp;
    char type;
    long size;
    char *data;
    char *b64;
    const char *object;
    char path[PATH_MAX];

    object = param_string( param, 1 );
    if ( object == NULL || *object == '\0' )
    {
        errno = EINVAL;
        return tfalse;
    }

    type = com_path( object, path, sizeof(path) );
    if ( type != COM_FILE_EXECUTE )
    {
        if ( type == COM_FILE_LIB )
        {
            errno = EPERM;
        }
        return tfalse;
    }

    fp = fopen( path, "r" );
    if ( fp == NULL )
    {
        return tfalse;
    }

    fseek( fp, 0, SEEK_END );
    size = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    if ( size < 0 )
    {
        fclose( fp );
        return tfalse;
    }

    data = malloc( size + 1 );
    if ( data == NULL )
    {
        fclose( fp );
        return tfalse;
    }

    fread( data, 1, size, fp );
    data[size] = '\0';
    fclose( fp );

    b64 = b64_encode( data, size );
    free( data );

    if ( b64 == NULL )
    {
        return tfalse;
    }

    v = string2x( b64 );
    free( b64 );

    return v;
}

/* save[ object, content ] — save base64-encoded content to the component file */
talk_t _save( obj_t this, param_t param )
{
    FILE *fp;
    char type;
    int len;
    int clen;
    size_t dlen;
    char *data;
    const char *object;
    const char *content;
    char dir[PATH_MAX];
    char path[PATH_MAX];

    object = param_string( param, 1 );
    content = param_string( param, 2 );
    if ( object == NULL || *object == '\0' || content == NULL || *content == '\0' )
    {
        errno = EINVAL;
        return tfalse;
    }

    type = com_path( object, path, sizeof(path) );
    if ( type != COM_FILE_EXECUTE )
    {
        if ( type == COM_FILE_LIB )
        {
            errno = EPERM;
        }
        return tfalse;
    }

    internal2path( dir, sizeof(dir), NULL );
    dlen = strlen( dir );
    if ( dlen > 0 && dir[dlen - 1] == '/' )
    {
        dlen--;
    }
    if ( strncmp( path, dir, dlen ) != 0 || path[dlen] != '/' )
    {
        errno = EPERM;
        return tfalse;
    }

    len = 0;
    data = b64_decode( content, &len );
    if ( data == NULL || len <= 0 )
    {
        return tfalse;
    }
    /* EVP_DecodeBlock counts '=' padding as trailing zero bytes */
    clen = strlen( content );
    if ( len > 0 && clen > 0 && content[clen - 1] == '=' )
    {
        len--;
    }
    if ( len > 0 && clen > 1 && content[clen - 2] == '=' )
    {
        len--;
    }
    if ( len <= 0 )
    {
        free( data );
        return tfalse;
    }

    fp = fopen( path, "w" );
    if ( fp == NULL )
    {
        free( data );
        return tfalse;
    }

    fwrite( data, 1, len, fp );
    fclose( fp );
    free( data );
    chmod( path, 0755 );

    return ttrue;
}

/* list[] — list shell components with path and methods */
talk_t _list( obj_t this, param_t param )
{
    FILE *fp;
    int n;
    talk_t ret;
    talk_t list;
    talk_t axp;
    talk_t item;
    talk_t methods;
    const char *p;
    const char *object;
    const char *path;
    char name[NAME_MAX];
    char line[512];

    list = com_list( SCRIPT_PROJECT, NULL );
    if ( list == NULL )
    {
        return NULL;
    }
    ret = json_create( NULL );
    if ( ret == NULL )
    {
        talk_free( list );
        return NULL;
    }

    axp = NULL;
    while ( NULL != ( axp = json_next( list, axp ) ) )
    {
        object = axp_name( axp );
        path = axp_string( axp );
        if ( object == NULL || path == NULL )
        {
            continue;
        }

        item = json_create( NULL );
        methods = json_create( NULL );
        if ( item == NULL || methods == NULL )
        {
            talk_free( item );
            talk_free( methods );
            continue;
        }
        json_set_string( item, "path", path );

        fp = fopen( path, "r" );
        if ( fp != NULL )
        {
            while ( fgets( line, sizeof(line), fp ) != NULL )
            {
                p = line;
                while ( *p == ' ' || *p == '\t' )
                {
                    p++;
                }
                if ( *p == '#' || *p == '\0' || *p == '\n' || *p == '\r' )
                {
                    continue;
                }
                if ( !( ( *p >= 'A' && *p <= 'Z' ) || ( *p >= 'a' && *p <= 'z' ) || *p == '_' ) )
                {
                    continue;
                }
                n = 0;
                while ( n < (int)sizeof(name) - 1 )
                {
                    if ( ( *p >= 'A' && *p <= 'Z' ) || ( *p >= 'a' && *p <= 'z' )
                            || ( *p >= '0' && *p <= '9' ) || *p == '_' )
                    {
                        name[n] = *p;
                        n++;
                        p++;
                    }
                    else
                    {
                        break;
                    }
                }
                name[n] = '\0';
                while ( *p == ' ' || *p == '\t' )
                {
                    p++;
                }
                if ( n > 0 && *p == '(' && name[0] != '_' )
                {
                    json_set_string( methods, name, "" );
                }
            }
            fclose( fp );
        }

        json_set_value( item, "methods", methods );
        json_set_value( ret, object, item );
    }

    talk_free( list );
    return ret;
}



