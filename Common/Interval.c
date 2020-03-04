/*! \file    Interval.c
 *  \brief   Implementation of floating point intervals.
 *  \author  Peter C. Chapin <pchapin@vtc.edu>
 */

#include "Interval.h"

void Interval_init( Interval *self, double min, double max )
{
    self->min = min;
    self->max = max;
}
