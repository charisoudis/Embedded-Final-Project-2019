#include "sampling_gettimeofday.h"

void sample_gettimeofday( double *timestampsArray, const double samplingPeriod, unsigned int *actualSampleIndex )
{
    struct timeval start_t;

    // Take rest samples
    *actualSampleIndex = 0;
    do
    {
        gettimeofday(&start_t, NULL);
        timestampsArray[*actualSampleIndex] = start_t.tv_usec/1e6 + start_t.tv_sec;

        usleep((__useconds_t) (samplingPeriod * 1e6));

        ++( *actualSampleIndex );
    }
    while( 1 );

}
