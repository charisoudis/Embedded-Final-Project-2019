#ifndef FINAL_CLIENT_H
#define FINAL_CLIENT_H

#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <zconf.h>
#include "utils.h"
#include "server.h"

#ifndef CLIENT_AEM
    #define CLIENT_AEM 9026
#endif

#ifndef CLIENT_AEM_RANGE
    #define CLIENT_AEM_RANGE_MIN 8000
    #define CLIENT_AEM_RANGE_MAX 9050
#endif

#ifndef PRODUCER_DELAY_RANGE
    #define PRODUCER_DELAY_RANGE_MIN 1
    #define PRODUCER_DELAY_RANGE_MAX 5
#endif

/// \brief Polling thread. Starts polling to find active servers. Creates a new thread for each server found.
void *polling_worker(void);

/// \brief Message producer thread. Produces a random message at the end of the pre-defined interval.
void *producer_worker(void);

#endif //FINAL_CLIENT_H
