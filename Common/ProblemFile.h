/*! \file    ProblemFile.h
    \brief   Definition of an abstract data type that handles problem instance files.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef PROBLEMFILE_H
#define PROBLEMFILE_H

#include <stdlib.h>

enum ProblemFileStatus {
    SUCCESS,
    CANNOT_OPEN,
    INVALID_FORMAT,
    MISSING_VERSION,
    UNEXPECTED_VERSION,
    DUPLICATE_SETTING
};

//! Structure that represents a problem instance file.
/*!
 *  This structure leaves the file uninterpreted. It is responsible for only reading and parsing
 *  the file. The meaning of the various settings is left for someone else to decide. This
 *  structure does enforce the rule that requires the Version setting to be first.
 */
typedef struct {


} ProblemFile;

enum ProblemFileStatus ProblemFile_initialize( ProblemFile *pf, const char *file_name );

#endif
