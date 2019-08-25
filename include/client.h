#ifndef FINAL_CLIENT_H
#define FINAL_CLIENT_H

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>

/// \brief Polling thread. Starts polling to find active servers. Creates a new thread for each server found.
void *polling_worker(void);

/// \brief Message producer thread. Produces a random message at the end of the pre-defined interval.
void *producer_worker(void);

#endif //FINAL_CLIENT_H
