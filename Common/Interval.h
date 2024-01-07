/*! \file    Interval.h
 *  \brief   Handling of floating point intervals.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef INTERVAL_H
#define INTERVAL_H

//! Structure that represents an interval of two doubles.
typedef struct {
	double min;
	double max;
} Interval;

void Interval_init( Interval *self, double min, double max );

//! Structure that represents a rectangular region of space.
typedef struct {
    Interval x_interval;
    Interval y_interval;
    Interval z_interval;
} Box;

#endif
