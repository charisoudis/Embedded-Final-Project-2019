#include "sampling_cputime.h"

void sample_cputime( double *timestampsArray, const double samplingPeriod, unsigned int *actualSampleIndex )
{
    struct timespec start_t;

    // Take samples
    *actualSampleIndex = 0;
    do
    {

        clock_gettime(CLOCK_MONOTONIC_RAW, &start_t);
        timestampsArray[*actualSampleIndex] = start_t.tv_nsec/1e9 + start_t.tv_sec;

        usleep((__useconds_t) (samplingPeriod * 1e6));

        ++( *actualSampleIndex );
    }
    while( 1 );

}