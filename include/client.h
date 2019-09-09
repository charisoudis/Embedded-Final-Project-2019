#ifndef FINAL_CLIENT_H
#define FINAL_CLIENT_H

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

/// \brief Fetches AEM of running device from "wlan0" network interface
/// \return uint32_t ( 4-digits )
uint32_t getClientAem(void);

/// \brief Polling thread. Starts polling to find active servers. Creates a new thread for each server found.
void *polling_worker(void);

/// \brief Message producer thread. Produces a random message at the end of the pre-defined interval.
void *producer_worker(void);

#endif //FINAL_CLIENT_H
