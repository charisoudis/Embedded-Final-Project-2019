#include "stats.h"

double stats_mean(double *array, unsigned int n)
{
    double sum = 0.0;
    int i = 0;

    do
    {
        sum += array[i];
    }
    while( ++i < n );

    return sum / n;
}

double stats_median(double *array, unsigned int n)
{
    if( n%2 == 0 )
    {
        // if there is an even number of elements, return mean of the two elements in the middle
        return ( array[n/2] + array[n/2-1] ) / 2.0;
    }
    else
    {
        // else return the element in the middle
        return array[n/2];
    }
}

double stats_deviation(double *array, unsigned int n, double mean)
{
    double sum = 0.0, deviation = 0.0;
    int i = 0;

    do
    {
        deviation += pow(array[i] - mean, 2);
    }
    while( ++i < n );

    return sqrt( deviation/n );
}

double stats_min(double *array, unsigned int n)
{
    return array[0];
}

double stats_max(double *array, unsigned int n)
{
    return array[n-1];
}
