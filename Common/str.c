/*! \file str.c
 *  \brief Definitions of the string methods.
 *  \author Peter C. Chapin <PChapin@vtc.vsc.edu>
 *  \date September 11, 2015
 *  \warning This code has not been extensively tested!
 *
 * The abstract type string is a C implementation of a dynamic string type. If you use the
 * methods defined here to manipulate string objects, strings will take care of their own memory
 * management, growing and sometimes shrinking as necessary to hold whatever data is put into
 * them.
 *
 * These strings have value semantics. When a copy of a string is made, the new string is
 * independent of the original. These strings can hold any data, including embedded null
 * characters. Strings without embedded nulls can be converted into a traditional C-style
 * pointer to char. The string can even be modified after the conversion provided that no
 * attempt is made to change its size.
 *
 * Ordinarily the methods of string return a value zero if they are successful. If they fail,
 * for example due to a lack of memory, they return -1. In that case they leave target objects
 * unmodified unless otherwise stated. Some methods assign other meanings to their return
 * values. Those meanings are explicitly documented with the method.
 *
 * Attempts to access a string using an out of range index causes undefined behavior. A future
 * version of this module may support some form of bounds checking, perhaps activated as a
 * compile time option.
 *
 * This implementation of strings is fairly standard. More memory is typically allocated for a
 * string than the string actually needs. When the extra memory is consumed, the total
 * allocation is doubled. This approach yields amortized constant time for the basic append
 * operation.
 *
 * These strings are currently not reference counted. Each string is maintained independently of
 * the others. This is easier to implement correctly, particularly in a multithreaded situation,
 * than reference counting.
 *
 * TO DO:
 *
 * + Review the formatted copy and append functions. Their handling of dynamic memory is not all
 *   that it should be. See the comments in the functions for more information.
 *
 * + Review the code for thread safety. Document findings (or fix unsafe parts).
 *
 * + Implement the other four relational operators.
 *
 * + Consider changing to a reference counted implementation. Watch thread safety issues.
 *   (Probably should document performance characteristics first to be sure such a change is
 *   worthwhile)
 *
 * + Write a decent test program.
 *
 * + Rewrite string_findchar() using a memory search function.
 *
 * + Consider implementing a more sophisticated string search algorithm in string_findstring().
 *
 * + Considering implementing some additional operations (Reverse string searches? Character
 *   replacement?)
 */

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"

// Small enough to be easy on memory. Large enough to be useful.
#define INITIAL_CAPACITY 8

//-----------------------------
//      Internal Functions
//-----------------------------

// The following function returns the smallest power of 2 that is greater than new_size (minimum
// of INITIAL_CAPACITY). This function is used to compute an appropriate new capacity in cases
// where that is necessary. This function makes no provision for overflow.
//
// It is necessary to return a value greater than new_size to allow space for a terminating null
// byte (without requiring any additional allocations) when it is needed.
//
static size_t round_up( size_t new_size )
{
    size_t size = INITIAL_CAPACITY;

    while( size <= new_size ) size <<= 1;
    return size;
}

//-----------------------------
//      External Functions
//-----------------------------


//! Create an empty string
/*!
 * In general do not attempt to use a string that has not initialized successfully. However, as
 * an exception, it is safe to call string_destroy() on such a string. If the string failed to
 * initialize properly such a call has no effect. Note that it is still undefined to call
 * string_destroy() on a string that has never had string_construct() called on it.
 *
 * \param object Pointer to the string to be initialized.
 *
 * \return -1 if the string failed to initialize due to a lack of memory resources; zero if the
 * initialization was successful.
 */
int string_construct( string *object )
{
    object->start    = (char *)malloc( INITIAL_CAPACITY );
    object->size     = 0;
    object->capacity = INITIAL_CAPACITY;
    if( object->start == NULL ) return -1;
    return 0;
}


//! Release the resources associated with a string.
/*!
 * Do not attempt to use a string after destruction. However, as an exception you can
 * reinitialize it with string_construct(). Note that it is not an error to call string_destroy()
 * on a string for which string_construct() failed. However, if string_construct() fails, calling
 * string_destroy() is optional.
 *
 * This method recovers the memory resources used by a string.
 *
 * \param object Pointer to the string to be destroyed.
 */
void string_destroy( string *object )
{
    free( object->start );
    object->start    = NULL;
    object->size     = 0;
    object->capacity = INITIAL_CAPACITY;
}


//! Make the target string into an empty string.
/*!
 * This method removes the contents of the target string. The string is left in a usable state.
 * If this method fails (which is very unlikely) the target string is left unchanged.
 *
 * This method reduces the capacity of the target string to the capacity used when a string is
 * first initialized.
 *
 * \param object Points to the string to be erased.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the target string is not changed.
 */
int string_erase( string *object )
{
    char *temp = (char *)malloc( INITIAL_CAPACITY );
    if( temp == NULL ) return -1;

    free( object->start );
    object->start    = temp;
    object->start[0] = '\0';  // Only needed to keep splint happy.
    object->size     = 0;
    object->capacity = INITIAL_CAPACITY;
    return 0;
}


//! Copy strings.
/*!
 * \param object Destination string. Overwritten by the copy.
 * \param other Source string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_copy( string *object, const string *other )
{
    size_t  new_capacity;
    char   *temp;

    assert( other->start != NULL );
    assert( object->start != NULL );

    // If I'm big enough to hold the other, just do the copy.
    if ( other->size < object->capacity ) {
        memcpy( object->start, other->start, other->size );
        object->size = other->size;
    }

    // Otherwise, reallocate myself.
    else {
        new_capacity = round_up( other->size + 1 );
        temp = (char *)malloc( new_capacity );
        if( temp == NULL ) return -1;
        free( object->start );
        object->start = temp;
        memcpy( object->start, other->start, other->size );
        object->size = other->size;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Copies a null terminated C style string into a string.
/*!
 * If the source string is NULL, the destination string is left unchanged. If the source string
 * is empty, the destination string is erased (but there is no change to the capacity of the
 * destination string).
 *
 * \param object Destination string. Overwritten by the copy.
 * \param other Pointer to a null terminated C style string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_copycharp( string *object, const char *other )
{
    size_t  other_size;
    size_t  new_capacity;
    char   *temp;

    if( other == NULL ) return 0;
    assert( object->start != NULL );

    other_size = strlen( other );

    // If I'm big enough to hold the other, just do the copy.
    if( other_size < object->capacity ) {
        memcpy( object->start, other, other_size );
        object->size = other_size;
    }

    // Otherwise, reallocate myself.
    else {
        new_capacity = round_up( other_size + 1 );
        temp = (char *)malloc( new_capacity );
        if( temp == NULL ) return -1;
        free( object->start );
        object->start = temp;
        memcpy( object->start, other, other_size );
        object->size = other_size;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Copy a character into a string.
/*!
 * \param object Destination string. Overwritten by the copy.
 * \param other Character to copy.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 * The capacity of the destination string is not modified by this operation.
 */
int string_copychar( string *object, char other )
{
    assert( object->start != NULL );
    
    // This version makes no attempt to reduce capacity.
    *object->start = other;
    object->size  = 1;
    return 0;
}


//! Copy a formatted string into a string.
/*!
 * The format string can contain format specifiers in the style of the standard printf()
 * function. Additional parameters must be provided as required by the format string. The result
 * of the formatting operation is placed into the destination string.
 *
 * \param object Destination string. Overwritten by the copy.
 * \param format printf() style format string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_copyf( string *object, const char *format, ... )
{
    // This is nasty and hacked. A better version would resize the buffer until it worked and it
    // would then install the buffer directly into the target object (no need to copy it).
    // Something to fix later...

    int     return_value;
    va_list args;
    char   *buffer = (char *)malloc( 1024 );

    if( buffer == NULL ) return -1;

    va_start( args, format );
    return_value = vsnprintf( buffer, 1024, format, args );
    if( return_value >= 1024 ) {
        free( buffer );
        return -1;
    }
    assert( return_value >= 0 );
    return_value = string_copycharp( object, buffer );
    free( buffer );
    va_end( args );

    return return_value;
}


//! Append one string onto another.
/*!
 * A copy of the source string is appended to the end of the destination string.
 *
 * \param object Destination string.
 * \param other Source spcia-string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_append( string *object, const string *other )
{
    size_t  new_capacity;
    char   *temp;

    assert( other->start != NULL );
    assert( object->start != NULL );

    // If the capacity is sufficient, just append.
    if( object->size + other->size < object->capacity ) {
        memcpy( object->start + object->size, other->start, other->size );
        object->size += other->size;
    }
    
    // Otherwise, reallocate myself.
    else {
        new_capacity = round_up( object->size + other->size + 1 );
        temp = ( char * )malloc( new_capacity );
        if( temp == NULL ) return -1;
        memcpy( temp, object->start, object->size );
        free( object->start );
        object->start = temp;
        memcpy( object->start + object->size, other->start, other->size );
        object->size += other->size;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Append a null terminated C style string onto the end of a string.
/*!
 * A copy of the source string is appended to the end of the destination string. If the source
 * string is NULL, the destination string is left unchanged.
 *
 * \param object Destination string.
 * \param other Points at a null terminated C style string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_appendcharp( string *object, const char *other )
{
    size_t  other_size;
    size_t  new_capacity;
    char   *temp;

    assert( object->start != NULL );
    
    if( other == NULL ) return 0;
    other_size = strlen( other );

    // If the capacity is sufficient, just append.
    if( object->size + other_size < object->capacity ) {
        memcpy( object->start + object->size, other, other_size );
        object->size += other_size;
    }

    // Otherwise, reallocate myself.
    else {
        new_capacity = round_up( object->size + other_size + 1 );
        temp = (char *)malloc( new_capacity );
        if( temp == NULL ) return -1;
        memcpy( temp, object->start, object->size );
        free( object->start );
        object->start = temp;
        memcpy( object->start + object->size, other, other_size );
        object->size += other_size;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Append a character onto the end of a string.
/*!
 * \param object Destination string.
 * \param other Character to append.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_appendchar( string *object, char other )
{
    size_t  new_capacity;
    char   *temp;

    assert( object->start != NULL );

    // If the capacity is sufficient, just append.
    if( object->size + 1 < object->capacity ) {
        *( object->start + object->size ) = other;
        object->size++;
    }

    // Otherwise reallocate myself.
    else {
        new_capacity = round_up( object->size + 1 + 1 );
        temp = (char *)malloc( new_capacity );
        if( temp == NULL ) return -1;
        memcpy( temp, object->start, object->size );
        free( object->start );
        object->start = temp;
        *( object->start + object->size ) = other;
        object->size++;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Append a formatted string onto the end of a string.
/*!
 * The format string can contain format specifiers in the style of the standard printf()
 * function. Additional parameters must be provided as required by the format string. The result
 * of the formatting operation is appended onto the end of the destination string.
 *
 * \param object Destination string.
 * \param format Format string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_appendf( string *object, const char *format, ... )
{
    // This is nasty and hacked. A better version would resize the buffer until it worked. It
    // would be nice if we could also avoid the final copy of the buffer into the target object.

    int     return_value;
    va_list args;
    char   *buffer = (char *)malloc( 1024 );

    if( buffer == NULL ) return -1;

    va_start( args, format );
    return_value = vsnprintf( buffer, 1024, format, args );
    if( return_value >= 1024 ) {
        free( buffer );
        return -1;
    }
    assert( return_value >= 0 );
    return_value = string_appendcharp( object, buffer );
    free( buffer );
    va_end( args );
    
    return return_value;
}


//! Prepend one string onto another.
/*!
 * A copy of the source string is inserted at the beginning of the destination string.
 *
 * \param object Destination string.
 * \param other Source string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_prepend( string *object, const string *other )
{
    char  *temp;
    size_t new_capacity;

    assert( object->start != NULL );
    assert( other ->start != NULL );
    
    // If there is sufficient space, don't allocate a new buffer.
    if( object->capacity - object->size > other->size ) {
        memmove( object->start + other->size, object->start, object->size );
        memcpy( object->start, other->start, other->size );
        object->size += other->size;
    }
    else {
        new_capacity = round_up( object->size + other->size );
        if( ( temp = ( char * )malloc( new_capacity ) ) == NULL ) {
            return -1;
        }
        memcpy( temp, other->start, other->size );
        memcpy( temp + other->size, object->start, object->size );

        // I was able to create the temp workspace correctly... now gut it.
        free( object->start );
        object->start    = temp;
        object->size    += other->size;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Prepend a null terminated C style string onto a string.
/*!
 * A copy of the source string is inserted at the beginning of the destination string. If the
 * source string is a NULL pointer there is no effect.
 *
 * \param object Destination string.
 * \param other Points at a null terminated C style string.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_prependcharp( string *object, const char *other )
{
    char  *temp;
    size_t new_capacity;
    size_t other_size;

    assert( object->start != NULL );
    if( other == NULL ) return 0;
    
    other_size = strlen( other );
   
    // If there is sufficient space, don't allocate a new buffer.
    if( object->capacity - object->size > other_size ) {
        memmove( object->start + other_size, object->start, object->size );
        memcpy( object->start, other, other_size );
        object->size += other_size;
    }
    else {
        new_capacity = round_up( object->size + other_size );
        if( ( temp = ( char * )malloc( new_capacity ) ) == NULL ) {
            return -1;
        }
        memcpy( temp, other, other_size );
        memcpy( temp + other_size, object->start, object->size );

        // I was able to create the temp workspace correctly... now gut it.
        free( object->start );
        object->start    = temp;
        object->size    += other_size;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Prepend a character onto a string.
/*!
 * \param object Destination string.
 * \param other Character to prepend.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if the
 * operation was successful. If the operation fails, the destination string is left unchanged.
 */
int string_prependchar( string *object, char other )
{
    char  *temp;
    size_t new_capacity;

    assert( object->start != NULL );    
   
    // If there is sufficient space, don't allocate a new buffer.
    if( object->capacity - object->size > 1 ) {
        memmove( object->start + 1, object->start, object->size );
        *( object->start ) = other;
        object->size += 1;
    }
    else {
        new_capacity = round_up( object->size + 1 );
        if( ( temp = ( char * )malloc( new_capacity ) ) == NULL ) {
            return -1;
        }
        *temp = other;
        memcpy( temp + 1, object->start, object->size );

        // I was able to create the temp workspace correctly... now gut it.
        free( object->start );
        object->start    = temp;
        object->size    += 1;
        object->capacity = new_capacity;
    }
    return 0;
}


//! Get the length of a string.
/*!
 * Strings can contain embedded null characters. Such characters are counted as part of the
 * string's length. There is no terminating null.
 *
 * \param object string to measure.
 *
 * \return The length of the string.
 */
size_t string_length( const string *object )
{
  return object->size;
}


//! Look up a character by its index.
/*!
 * Attempting to access a character position off the end of the string results in undefined
 * behavior.
 *
 * \param object Spcia-string to access.
 * \param char_index Index to desired character.
 *
 * \return Character at the specified index.
 */
char string_getcharat( const string *object, size_t char_index )
{
    assert( object->start != NULL );
    assert( char_index < object->size );
    return *( object->start + char_index );
}


//! Return a pointer to a null terminated C style string.
/*!
 * The pointer returned will point at a properly null terminated string even though the
 * terminating null character is not actually part of the string. Modifing the characters in the
 * string's internal representation using this pointer will change the string's value. The
 * pointer returned by this method will be invalidated by any operation that changes the length
 * of the string.
 *
 * \param object string to access.
 *
 * \return Pointer to the string's internal representation.
 */
char *string_getcharp( string *object )
{
    assert( object->start != NULL );
    
    *( object->start + object->size ) = '\0';
    return object->start;
}


//! Modify a character in a string.
/*!
 * This method replaces the character in the string at the specified index with the specified
 * character. Attempting to access a character position off the end of the string results in
 * undefined behavior. This method can not be used to change the length of a string.
 *
 * \param object string to access.
 * \param other Character to put into the string.
 * \param char_index Index into the string where character is to go.
 */
void string_putcharat(
  string *object, char other, size_t char_index )
{
    assert( object->start != NULL );
    assert( char_index < object->size );
    *( object->start + char_index ) = other;
}


//! Compare two strings for equality.
/*!
 * The comparison is done in a case sensitive way. Embedded null characters, if any, are handled
 * like any other character. They do not terminate the comparison.
 *
 * \param left First operand.
 * \param right Second operand.
 *
 * \return true if the two strings are identical; false otherwise.
 */
int string_equal( const string *left, const string *right )
{
    assert( left->start != NULL );
    assert( right->start != NULL );
    if( left->size != right->size ) return 0;
    if( memcmp( left->start, right->start, left->size ) != 0 ) return 0;
    return 1;
}


//! Compare two strings for the less than relationship.
/*!
 * The first character where the two strings are different has a lower ASCII code in the string
 * that is less.
 *
 * \param left First operand.
 * \param right Second operand.
 *
 * \return True if the first operand is less than the second.
 */
int string_less( const string *left, const string *right )
{
    char   *leftp = left->start;
    char   *rightp = right->start;
    size_t  index;

    assert( left->start != NULL && right->start != NULL );
  
    for( index = 0; index < left->size; index++ ) {
        if( index >= right->size ) break;
        if( *leftp < *rightp ) return 1;
        if( *leftp > *rightp ) return 0;
        leftp++; rightp++;
    }

    // If the right side is longer, then the left side is a prefix.
    if( index < right->size ) {
        return 1;
    }
    return 0;
}


//! Search a string for a particular character.
/*!
 * \param haystack string to search.
 * \param needle Character to search for.
 *
 * \return Index of the first occurrence of the needle character or (size_t)-1 if the needle
 * character does not occur.
 */
size_t string_findchar( const string *haystack, char needle )
{
    char   *haystackp = haystack->start;
    size_t  index;

    assert( haystack->start != NULL );

    // Rewrite in terms of a memory search function. (Can't use strchr() because strings might
    // contain embedded null bytes).
    // 
    for( index = 0; index < haystack->size; index++ ) {
        if( *haystackp == needle ) return index;
    }
    return (size_t)-1;
}


//! Search a string for another string.
/*!
 * The returned index is the index of the first character of the needle string as it appears in
 * the haystack string. For example, searching for "foo" in "fizzfoo" returns 4.
 *
 * \param haystack string to search.
 * \param needle string to search for.
 *
 * \return Index of the first occurrence of the needle string or (size_t)-1 if the needle string
 * does not occur.
 */
size_t string_findstring( const string *haystack, const string *needle )
{
    char   *haystackp = haystack->start;
    size_t  index1;
    size_t  index2;

    assert(haystack->start != NULL);
    assert(needle->start != NULL);

    // An empty needle is an error. (Is this right?)
    if( needle->size == 0 ) return 0;

    for( index1 = 0; index1 < (haystack->size - needle->size + 1); index1++ ) {
        char *needlep = needle->start;

        for( index2 = index1; index2 < index1 + needle->size; index2++ ) {
            if( haystackp[index2] != *needlep ) break;
        }

        // Did the inner loop end naturally? If so, there is a match.
        if( index2 >= index1 + needle->size ) return index1;
    }
    return (size_t)-1;
}


//! Extract a substring from a string.
/*!
 * If the specified length goes off the end of the source string, the last part of the source
 * string will be taken and the resulting substring will not be the specified length. If the
 * initial index is out of bounds, the behavior is undefined.
 *
 * \param source string to access.
 * \param target string where substring will be placed.
 * \param index Starting index of substring in source.
 * \param length Size of substring.
 *
 * \return -1 if the operation fails to complete due to a lack of memory resources; zero if it
 * is successful. If the operation fails the target string will contain a partial result (the
 * part of the substring that was copied there before the failure).
 */
int string_substring(
  const string *source,
        string *target,
        size_t        index,
        size_t        length )
{
    size_t temp_index;

    assert( source->start != NULL );
    assert( target->start != NULL );
    assert( index + length < source->size );

    if( string_erase(target) == -1 ) return -1;
    for( temp_index = index; temp_index < index + length; temp_index++ ) {
        if( temp_index >= source->size ) break;
        if( string_appendchar( target, source->start[temp_index] ) == -1 ) return -1;
    }
    return 0;
}


//! Reverses the characters in the string.
/*!
 * This method reverses the order in which the characters appear in the string. All characters
 * participate in the reversing, including leading and trailing spaces.
 *
 * \param object The string to reverse.
 */
void string_reverse( string *object )
{
    char *left;
    char *right;
    assert( object->start != NULL );

    left = object->start;
    right = (object->start + object->size) - 1;

    while( left < right ) {
        char temp = *left;
        *left = *right;
        *right = temp;
        ++left;
        --right;
    }
}


//! Swaps the contents of two string objects.
/*!
* This method swaps two strings in O(1) time.
*
* \param left The first string to swap.
* \param right The second string to swap.
*/
void string_swap( string *left, string *right )
{
    char  *temp_pointer;
    size_t temp_size;
    assert( left != NULL && right != NULL );

    temp_pointer = left->start;
    left->start = right->start;
    right->start = temp_pointer;

    temp_size = left->size;
    left->size = right->size;
    right->size = temp_size;

    temp_size = left->capacity;
    left->capacity = right->capacity;
    right->capacity = temp_size;
}


//! Read a white space delimited word from an input file.
/*!
 * This method returns the next white space delimited word from the input file. Leading white
 * space is skipped. If this method returns due to an error condition, the destination string
 * will contain the characters successfully read before the error occured. All other
 * chararacters will remain on the input stream (no characters are lost).
 *
 * \param object Destination string.
 * \param infile Input file.
 *
 * \return -1 if the end of file is encountered without any characters being loaded into the
 * destination string OR if an error occurs due to a lack of memory resources; zero if a string
 * was successfully read.
 */
/*@+charint@*/
int string_read( string *object, FILE *infile )
{
    int ch;
    int ungot_ch;

    assert( object->start != NULL );
    assert( infile != NULL );

    if( string_erase( object ) == -1 ) return -1;

    // Skip leading white space.
    while( ( ch = getc( infile ) ) != EOF ) {
        if( ch != ' ' && ch != '\t' && ch != '\f' && ch != '\n' ) {
            ungot_ch = ungetc( ch, infile );
            assert( ungot_ch != EOF );
            break;
        }
    }

    // Read the next "word"
    while( ( ch = getc( infile ) ) != EOF ) {
        if( ch == ' ' || ch == '\t' || ch == '\f' || ch == '\n' ) {
            ungot_ch = ungetc( ch, infile );
            assert( ungot_ch != EOF );
            break;
        }
        if( string_appendchar(object, (char)ch ) == -1 ) {
            ungot_ch = ungetc( ch, infile );
            assert( ungot_ch != EOF );
            return -1;
        }
    }
    
    return ( ch == EOF && object->size == 0 ) ? -1 : 0;
}
/*@=charint@*/


//! Read a line of text from an input file.
/*!
 * This method reads an entire line (of arbitrary length) from the input file. It consumes the
 * newline character in the input stream but it does not store the newline character in the
 * string. If this method returns due to an error condition, the destination string will contain
 * the characters successfully read before the error occurred. All other characters will remain
 * on the input stream.
 *
 * \param object Destination string.
 * \param infile Input file.
 *
 * \return -1 if the end of file is encountered without any characters being loaded into the
 * destination string OR if an error occurs due to a lack of memory resources; zero if a string
 * was successfully read.
 */
/*@+charint@*/
int string_readline( string *object, FILE *infile )
{
    int ch;
    int ungot_ch;

    assert( object->start != NULL );
    assert( infile != NULL );

    if( string_erase( object ) == -1 ) return -1;
    while( ( ch = getc( infile ) ) != EOF ) {
        if( ch == '\n' ) break;
        if( string_appendchar( object, (char)ch ) == -1 ) {
            ungot_ch = ungetc( ch, infile );
            assert( ungot_ch != EOF );
            return -1;
        }
    }

    return ( ch == EOF && object->size == 0 ) ? -1 : 0;
}
/*@=charint@*/


//! Write a string to an output file.
/*!
 * \param object string to write.
 * \param outfile Output file.
 *
 * \return -1 if an error occurs during the write operation; zero otherwise.
 */
int string_write( const string *object, FILE *outfile )
{
    char   *objectp = object->start;
    size_t  index;

    assert( object->start != NULL );
    assert( outfile != NULL );

    for( index = 0; index < object->size; index++ ) {
        if( fputc( *objectp, outfile ) == EOF ) return -1;
        objectp++;
    }

    return 0;
}


//! Write a string and a newline to an output file.
/*!
 * This method differs from string_write() in that it also writes a newline character to the
 * output file after it has writen the specified string. This method is intended to work with
 * string_readline().
 *
 * \param object string to write.
 * \param outfile Output file.
 *
 * \return -1 if an error occurs during the write operation; zero otherwise.
 */
int string_writeline( const string *object, FILE *outfile )
{
    assert( object->start != NULL );
    assert( outfile != NULL );

    if( string_write( object, outfile ) == -1 ) return -1;
    if( fputc( '\n', outfile ) == EOF ) return -1;
    return 0;
}
