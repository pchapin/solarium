
CUDA Version
============

This folder contains a CUDA version of the Solarium program. Because the GPU only supports
single precision floating point types, it was necessary to replicate (and adjust) the
initialization and Vector3 libraries here rather than using the version in the Common folder and
the Spica library.

It is also necessary to deal with potential overflows in the calculation of the gravitational
force. Consider Newton's law of gravity:

    F = ( G * M_1 * M_2 ) / D^2

The product M_1 * M_2 and the computation of D^2 can overflow the range of float when the
standard MKS system of units is used. For example the mass of the Earth is about 6x10^24 kg so
M_1 * M_2 could be on the order of 3.6x10^49 kg^2 (this example program uses numerous Earth
massed objects). An overflow of this nature results in a special NaN ("Not a Number") value
being produced. All computations on a NaN result in NaN and so the "answer" produced by the
program ends up being a large number of NaNs.

One way to avoid this problem, or at least reduce the likelihood of it arising, is to reorder
the computation as follows:

    F = (G * (M_1/D)) * (M_2/D)

Here dividing a large mass by a large distance produces a more reasonable value to participate
in the following calculations. This is the approach used by this example program.

Another approach would be to shift the system of units so that the computations do not overflow
that more limited range of float. For example, in this computation we could use:

    Distance unit: 1 astronomical unit (AU)
    Mass unit: 1 Earth mass (ME)
    Time unit: 1 day (Day)

Consider now the gravitational formula:

    F = ( G * M_1 * M_2 ) / D^2

Using MKS units:

    kg m s^{-2} = G kg^2 m^{-2}

Thus the units on G are:

    G = m^3 kg^{-1} s^{-2}

Conversion factors:

    1 AU  = 1.49597870700E+11 m
    1 ME  = 5.9722E+24 kg
    1 Day = 8.64E+4 s

In the new system the universal gravitational constant is:

    G = 6.673E-11 * (1/1.49597870700E+11)^3 * (1/5.9722E+24)^{-1} * (1/8.64E+4)^{-2}
      = 6.673E-11 * (1.49597870700E+11)^{-3} * 5.9722E+24 * (8.64E+4)^2
      = 6.673 * (1.49597870700)^{-3} * 5.9722 * (8.64)^2 * 1E-12
      = 6.673 * 0.298692119 * 5.9722 * 74.6496 * 1E-12
      = 8.88601E-10 AU^3 ME^{-1} Day^{-2}
