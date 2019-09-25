//
// Created by achariso on 31/3/2019.
//

#ifndef PROJECT1_SAMPLING_GETTIMEOFDAY_H
#define PROJECT1_SAMPLING_GETTIMEOFDAY_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/// @brief Performs sampling using gettimeofday() to retrieve timestamps.
/// @author Thanasis Charisoudis
///
/// @param timestampsArray - the timestamps array to fill with timestamps
/// @param samplingPeriod - the sampling period
/// @param actualSampleIndex - resulting number of samples
/// @return void
void sample_gettimeofday(double *, double, unsigned int *);

#endif //PROJECT1_SAMPLING_GETTIMEOFDAY_H
