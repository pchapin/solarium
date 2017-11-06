/*! \file    ProblemFile.c
    \brief   Implementation of an abstract data type that handles problem instance files.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <stdio.h>
#include <string.h>
#include "ProblemFile.h"
#include "str.h"

static int is_white( char ch )
{
    if( ch == ' ' || ch == '\t' ) return 1;
    return 0;
}

static void trim_leading_whitespace( string *s )
{
    const char *start = string_getcharp( s );
    const char *p     = start;
    size_t      length;
    string      result;

    string_construct( &result );
    while( *p && is_white( *p )) p++;
    length = string_length( s ) - ( p - start );
    string_substring( s, &result, ( p - start ), length );
    string_copy( s, &result );
    string_destroy( &result );
}

static void trim_trailing_whitespace( string *s )
{
    const char *start = string_getcharp( s );
    const char *p     = start + string_length( s ) - 1;
    size_t      length;
    string      result;

    string_construct( &result );

    // TODO: It is technically undefined to step p off the front of the string.
    while( p >= start && is_white( *p )) p--;
    length = ( p + 1 ) - start;
    string_substring( s, &result, 0, length );
    string_copy( s, &result );
    string_destroy( &result );
}

static void remove_comments( string *s )
{
    string result;

    size_t position = string_findchar( s, '#' );
    if( position != (size_t)-1 ) {
        string_construct( &result );
        string_substring( s, &result, 0, position );
        string_copy( s, &result );
        string_destroy( &result );
    }
}

//! Open and parse a problem instance file.
/*!
 * This function will return an error if the problem instance file has the wrong format or if
 * it lacks a Version setting or if the Version setting is not the first setting in the file.
 *
 * \param file_name The name of the problem instance file to process.
 */
enum ProblemFileStatus ProblemFile_initialize( ProblemFile *pf, const char *file_name )
{
    int    expecting_version = 1;
    FILE  *problem_file;
    string line;
    size_t split_position;
    string name;
    string value;

    if( (problem_file = fopen(file_name, "r")) == NULL ) {
        return CANNOT_OPEN;
    }

    string_construct( &line  );
    string_construct( &name  );
    string_construct( &value );

    while( string_readline( &line, problem_file ) != -1 ) {
        remove_comments( &line );
        trim_trailing_whitespace( &line );
        trim_leading_whitespace( &line );
        if( string_length( &line ) == 0 ) continue;

        // Break the line into 'name' and 'value' parts.
        split_position = string_findchar( &line, '=' );
        if( split_position == (size_t)-1 ) return INVALID_FORMAT;
        string_substring( &line, &name, 0, split_position );
        string_substring(
            &line,
            &value,
            split_position + 1,
            string_length( &line ) - (split_position + 1));
        trim_trailing_whitespace( &name );
        trim_leading_whitespace( &value );

        // Make sure the Version setting is first in the file.
        if( expecting_version && strcmp( string_getcharp( &name ), "Version" ) != 0 ) {
            return MISSING_VERSION;
        }
        else if( expecting_version && strcmp( string_getcharp( &name ), "Version" ) == 0 ) {
            expecting_version = 0;
            if( strcmp( string_getcharp( &value ), "1" ) != 0 ) {
                return UNEXPECTED_VERSION;
            }
        }

        // Install the setting in the settings map.
        // TODO: Implement a settings map.
        printf( "name = %s; value = %s\n", string_getcharp( &name ), string_getcharp( &value ) );
    }
    return SUCCESS;
}
