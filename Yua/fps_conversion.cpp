#include "fps_conversion.h"

quint64 FPS_Conversion::my_UInt32x32To64(quint32 a, quint32 b) {
        return a * b;
}

// Avisynth v2.5.  Copyright 2002 Ben Rudiak-Gould et al.
// http://www.avisynth.org

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
// http://www.gnu.org/copyleft/gpl.html .
//
// Linking Avisynth statically or dynamically with other modules is making a
// combined work based on Avisynth.  Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Avisynth give you
// permission to link Avisynth with independent modules that communicate with
// Avisynth solely through the interfaces defined in avisynth.h, regardless of the license
// terms of these independent modules, and to copy and distribute the
// resulting combined work under terms of your choice, provided that
// every copy of the combined work is accompanied by a complete copy of
// the source code of Avisynth (the version of Avisynth used to produce the
// combined work), being distributed under the terms of the GNU General
// Public License plus this exception.  An independent module is a module
// which is not derived from or based on Avisynth, such as 3rd-party filters,
// import and export plugins, or graphical user interfaces.


/*********************************************
 ******    Float and Rational utility   ******
 *********************************************

 *********************************************
 ******  Thanks to RAYMOD2 for his help  *****
 ******  in beating continued fractions  *****
 ******  into a usable form.       IanB  *****
 *********************************************/

// This function converts a 32-bit IEEE float into a fraction.  This
// process is lossless but it may fail for very large or very small
// floats.  It also discards the sign bit.  Since the denominator will
// always be a power of 2 and the numerator will always be odd (except
// when the denominator is 1) the fraction will already be reduced
// to its lowest terms. output range (2^32-2^(32-24))/1 -- (1/2^31)
// i.e. 4294967040/1 -- 1/2147483648 (4.65661287307e-10)
// returns true if input is out of range

bool FPS_Conversion::float_to_frac(float input, unsigned &num, unsigned &den)
{
  union { float f; unsigned i; } value;
  unsigned mantissa;
  int exponent;

  // Bit strip the float
  value.f = input;
  mantissa = (value.i & 0x7FFFFF) + 0x800000;  // add implicit bit on the left
  exponent = ((value.i & 0x7F800000) >> 23) - 127 - 23;  // remove decimal pt

  // minimize the mantissa by removing any trailing 0's
  while (!(mantissa & 1)) { mantissa >>= 1; exponent += 1; }

  // if too small try to pull result from the reciprocal
  if (exponent < -31) {
    return float_to_frac((float)(1.0/input), den, num);
  }

  // if exponent is too large try removing leading 0's of mantissa
  while( (exponent > 0) && !(mantissa & 0x80000000) ) {
     mantissa <<= 1; exponent -= 1;
  }
  if (exponent > 0) {  // number too large
    num = 0xffffffff;
    den = 1;
    return true; // Out of range!
  }
  num = mantissa;
  den = 1 << (-exponent);

  return false;
}


// This function uses continued fractions to find the rational
// approximation such that the result truncated to a float is
// equal to value. The semiconvergents for the smallest such
// rational pair is then returned. The algorithm is modified
// from Wikipedia, Continued Fractions.

bool FPS_Conversion::reduce_float(float value, unsigned &num, unsigned &den)
{
  if (float_to_frac(value, num, den)) return true;

  unsigned n0 = 0, n1 = 1, n2, nx=num;  // numerators
  unsigned d0 = 1, d1 = 0, d2, dx=den;  // denominators
  unsigned a2, ax, amin;  // integer parts of quotients
  unsigned f1 = 0, f2;    // fractional parts of quotients

  while (1)  // calculate convergents
  {
    a2 = nx / dx;
    f2 = nx % dx;
    n2 = n0 + n1 * a2;
    d2 = d0 + d1 * a2;

    if (f2 == 0) break;  // no more convergents (n2 / d2 == input)

    // Damn compiler does not correctly cast
    // to intermediate results to float
    float f = (float)((double)n2/d2);
    if (f == value) break;

    n0 = n1; n1 = n2;
    d0 = d1; d1 = d2;
    nx = dx; dx = f1 = f2;
  }
  if (d2 == 1)
  {
    num = n2;
    den = d2;
  }
  else { // we have been through the loop at least twice

    if ((a2 % 2 == 0) && (d0 * f1 > f2 * d1))
       amin = a2 / 2;  // passed 1/2 a_k admissibility test
    else
       amin = a2 / 2 + 1;

// find the sign of the error (actual + error = n2/d2) and then
// set (n2/d2) = (num/den + sign * epsilon) = R2 and solve for ax
//--------------------
//   n2 = n0 + n1 * ax
//   d2 = d0 + d1 * ax
//-----------------------------------------------
//   (n2/d2)       = R2     = (n0 + n1 * ax)/(d0 + d1 * ax)
//   n0 + n1 * ax           = R2 * (d0 + d1 * ax)
//   n0 + n1 * ax           = R2 * d0 + R2 * d1 * ax
//   R2 * d1 * ax - n1 * ax = n0 - R2 * d0
//   (R2 * d1 - n1) * ax    = n0 - R2 * d0
//   ax                     = (n0 - R2 * d0)/(R2 * d1 - n1)

    // bump float to adjacent float value
    union { float f; unsigned i; } eps; eps.f = value;
    if (my_UInt32x32To64(n1, den) > my_UInt32x32To64(num, d1))
      eps.i -= 1;
    else
      eps.i += 1;
    double r2 = eps.f;
    r2 += value;
    r2 /= 2;

    double yn = n0 - r2*d0;
    double yd = r2*d1 - n1;
    ax = (unsigned)((yn + yd)/yd); // ceiling value

    if (ax < amin) ax = amin;

    // calculate nicest semiconvergent
    num = n0 + n1 * ax;
    den = d0 + d1 * ax;
  }
  return false;
}


// This function uses continued fractions to find the best rational
// approximation that satisfies (denom <= limit).  The algorithm
// is from Wikipedia, Continued Fractions.
//
void FPS_Conversion::reduce_frac(unsigned &num, unsigned &den, unsigned limit)
{
  unsigned n0 = 0, n1 = 1, n2, nx = num;    // numerators
  unsigned d0 = 1, d1 = 0, d2, dx = den;  // denominators
  unsigned a2, ax, amin;  // integer parts of quotients
  unsigned f1 = 0, f2;        // fractional parts of quotients
  int i = 0;  // number of loop iterations

  while (1) { // calculate convergents
    a2 = nx / dx;
    f2 = nx % dx;
    n2 = n0 + n1 * a2;
    d2 = d0 + d1 * a2;

    if (f2 == 0) break;
    if ((i++) && (d2 >= limit)) break;

    n0 = n1; n1 = n2;
    d0 = d1; d1 = d2;
    nx = dx; dx = f1 = f2;
  }
  if (d2 <= limit)
  {
    num = n2; den = d2;  // use last convergent
  }
  else { // (d2 > limit)
    // d2 = d0 + d1 * ax
    // d1 * ax = d2 - d1
    ax = (limit - d0) / d1;  // set d2 = limit and solve for a2

    if ((a2 % 2 == 0) && (d0 * f1 > f2 * d1))
      amin = a2 / 2;  // passed 1/2 a_k admissibility test
    else
      amin = a2 / 2 + 1;

    if (ax < amin) {
      // use previous convergent
      num = n1;
      den = d1;
    }
    else {
      // calculate best semiconvergent
      num = n0 + n1 * ax;
      den = d0 + d1 * ax;
    }
  }
}


void FPS_Conversion::float_to_fps(float n, unsigned &num, unsigned &den) {
        float x;
        unsigned u = (unsigned)(n*1001+0.5);

        // Check for the 30000/1001 multiples
        x = (float)((u/30000*30000)/1001.0);
        if (x == n) { num = u; den= 1001; return; }

        // Check for the 24000/1001 multiples
        x = (float)((u/24000*24000)/1001.0);
        if (x == n) { num = u; den= 1001; return; }

        if (n < 14.986) {
                // Check for the 30000/1001 factors
                u = (unsigned)(30000/n+0.5);
                x = (float)(30000.0/(u/1001*1001));
                if (x == n) { num = 30000; den= u; return; }

                // Check for the 24000/1001 factors
                u = (unsigned)(24000/n+0.5);
                x = (float)(24000.0/(u/1001*1001));
                if (x == n) { num = 24000; den= u; return; }
        }

        // Find the rational pair with the smallest denominator
        // that is equal to n within the accuracy of an IEEE float.
        if (reduce_float(n, num, den)) {
                qDebug() << "float_to_fps(): fatal: fps is out of range";
                exit(1);
        }
}
