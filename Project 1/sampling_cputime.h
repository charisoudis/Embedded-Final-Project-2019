#ifndef PROJECT1_SAMPLING_CPUTIME_H
#define PROJECT1_SAMPLING_CPUTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/// @brief Performs sampling using clock_gettime() to retrieve timestamps from system's real time clock.
/// @author Thanasis Charisoudis
///
/// @param timestampsArray - the timestamps array to fill with timestamps
/// @param samplingPeriod - the sampling period
/// @param actualSampleIndex - resulting number of samples
/// @return void
void sample_cputime(double *, double, unsigned int *);

#endif //PROJECT1_SAMPLING_CPUTIME_H
