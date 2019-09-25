#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include "sampling_gettimeofday.h"
#include "sampling_cputime.h"
#include "stats.h"

unsigned int numberOfSamples, actualNumberOfSamples;
static double totalSamplingTime, samplingPeriod;
static char type;
double *timestampsArray, *samplingPeriodsArray, *statsArray;

/// \brief Handler of SIG_ALRM signal. Used to terminate sampling when totalSamplingTime finishes.
/// \author Thanasis Charisoudis
///
/// \param signo
/// \return void - Actually this function terminates program execution.
static void onAlarm(int);

/// \brief Writes sampling output to 3 text files: 1) TimestampsArray.txt, 2) SamplingPeriodsArray.txt & 3) Stats.txt
/// \author Thanasis Charisoudis
///
/// \param timestampsArray - array holding all taken timestamps
/// \param samplingPeriodsArray - array holding all recorded periods ( between consecutive samples )
/// \param statsArray - array containing [min, max, mean, median, deviation]
/// \param numberOfSamples - number of samples in this sampling realization
/// \param samplingPeriod - time period between consecutive samples
/// \param type - defines the sampling method ( either gettimeofday() or clock_gettime() )
/// \return void
void toTxt(double *, double *, double *, unsigned int, double, char);

/// \brief Compare function used be qsort().

/// \param a - left element of comparison
/// \param b - right element of comparison
/// \return 1 if a > b, 0 if equal, -1 otherwise
static int compare(const void * a, const void * b);

int main(int argc, char **argv)
{
    // Initial Data
    char *ptr;
    totalSamplingTime = strtod(argv[1], &ptr);
    samplingPeriod = strtod(argv[2], &ptr);
    numberOfSamples = (unsigned int) round(totalSamplingTime / samplingPeriod);
    type = (char) ((argc < 4 ) ? 'c' : argv[3][0]);

    // Allocate Memory
    timestampsArray = (double *) malloc( numberOfSamples * sizeof( double ) );
    samplingPeriodsArray = (double *) malloc( numberOfSamples * sizeof( double ) );
    statsArray = (double *) malloc( 5 * sizeof( double ) );

    // Setup alarm
    alarm((unsigned int) totalSamplingTime);
    signal(SIGALRM, onAlarm);

    // Sample using selected type
    if ( type == 'd' )
    {
        sample_gettimeofday(timestampsArray, samplingPeriod, &actualNumberOfSamples);
    }
    else
    {
        sample_cputime(timestampsArray, samplingPeriod, &actualNumberOfSamples);
    }

    return 0;
}

static int compare(const void * a, const void * b)
{
    double ad, bd;
    ad = *(double*)a;
    bd = *(double*)b;

    if ( ad == bd ) return 0;
    return ad > bd ? 1 : -1;
}

void toTxt(double* timestampsArray, double* samplingPeriodsArray, double* statsArray, unsigned int n,
           double samplingPeriod, char type)
{
    // 1) Write timestamps
    FILE* fp = fopen("TimestampsArray.txt", "a");  // open in append mode
    fprintf(fp, "--- New Sampling ( %d/%d samples - %.4f secs period - using %s ) ---\n",
            n + 1, numberOfSamples, samplingPeriod, type == 'c' ? "clock_gettime()" : "gettimeofday()");
    int i = 0;
    do
    {
        fprintf(fp, "%.10f\n", timestampsArray[i]);
    }
    while( ++i < ( n + 1 ) );
    fclose(fp);

    // 2) Write periods
    fp = fopen("SamplingPeriodsArray.txt", "a");  // open in append mode
    fprintf(fp, "--- New Sampling ( %d/%d samples - %.4f secs period - using %s ) ---\n",
            n, numberOfSamples - 1, samplingPeriod, type == 'c' ? "clock_gettime()" : "gettimeofday()");
    i = 0;
    do
    {
        fprintf(fp, "%.10f\n", samplingPeriodsArray[i]);
    }
    while( ++i < n );
    fclose(fp);

    // 3) Write stats
    fp = fopen("Stats.txt", "a");
    fprintf(fp, "--- New Sampling ( %d/%d samples - %.4f secs period - using %s ) ---\n",
            n, numberOfSamples, samplingPeriod, type == 'c' ? "clock_gettime()" : "gettimeofday()");
    fprintf(fp, "min:       %.10f \n", statsArray[0]);
    fprintf(fp, "max:       %.10f \n", statsArray[1]);
    fprintf(fp, "mean:      %.10f \n", statsArray[2]);
    fprintf(fp, "median:    %.10f \n", statsArray[3]);
    fprintf(fp, "deviation: %.10f \n", statsArray[4]);
    fclose(fp);

    printf("\t--> results written to respective text files\n");
}

static void onAlarm(int signo)
{
    fprintf(stdout, "Caught the SIGALRM signal\n");

    // Calculate periods
    int i = 0;
    do
    {
        samplingPeriodsArray[i] = timestampsArray[i+1] - timestampsArray[i];
    }
    while( ++i < actualNumberOfSamples - 1 );

    // Sort periods in ascending order ( periods are one less than samples )
    actualNumberOfSamples -= 1;

    // Sort array ( retaining old )
    double *samplingPeriodsArraySorted;
    samplingPeriodsArraySorted = malloc( actualNumberOfSamples * sizeof( double ) );
    memcpy( samplingPeriodsArraySorted, samplingPeriodsArray, actualNumberOfSamples * sizeof( double ) );

    qsort( samplingPeriodsArraySorted, actualNumberOfSamples, sizeof(double), compare );

    // Stats calculation
    statsArray[0] = stats_min(samplingPeriodsArraySorted, actualNumberOfSamples);
    statsArray[1] = stats_max(samplingPeriodsArraySorted, actualNumberOfSamples);
    statsArray[2] = stats_mean(samplingPeriodsArraySorted, actualNumberOfSamples);
    statsArray[3] = stats_median(samplingPeriodsArraySorted, actualNumberOfSamples);
    statsArray[4] = stats_deviation(samplingPeriodsArraySorted, actualNumberOfSamples, statsArray[2]);

    free( samplingPeriodsArraySorted );

    // Write to txt
    toTxt(timestampsArray, samplingPeriodsArray, statsArray, actualNumberOfSamples, samplingPeriod, type);

    // Free Memory
    free( timestampsArray );
    free( samplingPeriodsArray );
    free( statsArray );

    printf("Exiting now...\n");
    exit(EXIT_SUCCESS);
}

// terminal: cd cmake-build-debug && ./project1 7200 0.1 c && ./project1 7200 0.1 d