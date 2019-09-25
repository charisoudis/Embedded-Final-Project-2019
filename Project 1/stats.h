//
// Created by achariso on 31/3/2019.
//

#ifndef PROJECT1_STATS_H
#define PROJECT1_STATS_H

#include <stdlib.h>
#include <math.h>

/// \brief Computes the mean value of the values in array
/// \author Thanasis Charisoudis
///
/// \param array - SORTED array used for computations
/// \param n - number of elements in array
/// \return mean value of passed array
double stats_mean(double *, unsigned int);

/// \brief Computes the median value of the values in array
/// \author Thanasis Charisoudis
///
/// \param array - SORTED array used for computations
/// \param n - number of elements in array
/// \return median value of passed array
double stats_median(double *, unsigned int);

/// \brief Computes the standard deviation of the values in array
/// \author Thanasis Charisoudis
///
/// \param array - SORTED array used for computations
/// \param n - number of elements in array
/// \param mean - mean value of the values in array ( see stats_mean() )
/// \return standard deviation of the values in passed array
double stats_deviation(double *, unsigned int, double);

/// \brief Computes the min value of the values in array ( first element )
/// \author Thanasis Charisoudis
///
/// \param array - SORTED array used for computations
/// \param n - number of elements in array
/// \return min value of passed array
double stats_min(double *, unsigned int);

/// \brief Computes the max value of the values in array ( last element )
/// \author Thanasis Charisoudis
///
/// \param array - SORTED array used for computations
/// \param n - number of elements in array
/// \return max value of passed array
double stats_max(double *, unsigned int);

#endif //PROJECT1_STATS_H
