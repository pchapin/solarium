/*! \file str.h
 *  \brief Declarations of the string methods.
 *  \author Peter C. Chapin <PChapin@vtc.vsc.edu>
 *  \date September 11, 2015
 */

#ifndef STR_H
#define STR_H

#include <stddef.h>
#include <stdio.h>

//! Abstract type representing unbounded, dynamic strings.
typedef /*@abstract@*/ struct {
    /*@null@*/ char   *start;     //!< Start of string allocation.
               size_t  size;      //!< Number of characters in string.
               size_t  capacity;  //!< Number of bytes of reserved space.
} string;


#ifdef __cplusplus
extern "C" {
#endif

int    string_construct(/*@out@*/ string *object );
void   string_destroy( string *object );
int    string_erase( string *object );
int    string_copy( string *object, const string *other );
int    string_copycharp( string *object, /*@null@*/ const char *other );
int    string_copychar( string *object, char other );
int    string_copyf( string *object, const char *format, ... );
int    string_append( string *object, const string *other );
int    string_appendcharp( string *object, /*@null@*/ const char *other );
int    string_appendchar( string *object, char other );
int    string_appendf( string *object, const char *format, ... );
int    string_prepend( string *object, const string *other );
int    string_prependcharp( string *object, /*@null@*/ const char *other );
int    string_prependchar( string *object, char other );
size_t string_length( const string *object );
char   string_getcharat( const string *object, size_t char_index );
char  *string_getcharp( string *object );
void   string_putcharat( string *object, char other, size_t char_index );
int    string_equal( const string *left, const string *right );
int    string_less( const string *left, const string *right );
size_t string_findchar( const string *haystack, char needle );
size_t string_findstring( const string *haystack, const string *needle );
int    string_substring( const string *source, string *target, size_t  index, size_t  length );
void   string_reverse( string *object );
void   string_swap( string *left, string *right );
int    string_read( string *object, FILE *infile );
int    string_readline( string *object, FILE *infile );
int    string_write( const string *object, FILE *outfile );
int    string_writeline( const string *object, FILE *outfile );

#ifdef __cplusplus
}
#endif

#endif
