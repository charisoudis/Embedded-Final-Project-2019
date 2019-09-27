#ifndef FINAL_COMMUNICATION_H
#define FINAL_COMMUNICATION_H

#include "types.h"
#include "utils.h"

/// \brief Datetime transmitter loop. Transmits current datetime on each new connection.
void communication_datetime_listener_worker();

/// \brief Receive current "timeofday" from reference device ( with AEM = 0001 )
/// \return TRUE on success, FALSE on failure
bool communication_datetime_receiver();

/// \brief Handle communication staff with connected device ( POSIX thread compatible function ).
/// \param thread_args pointer to communicate_args_t type
void communication_worker(void *args);

/// \brief Receiver sub-worker of communication worker ( POSIX thread compatible function ).
/// \param connectedSocket socket file descriptor with connected device
/// \param connectedDevice connected device that will send messages
void communication_receiver_worker(int32_t connectedSocket, Device connectedDevice);

/// \brief Transmitter sub-worker of communication worker ( POSIX thread compatible function ).
/// \param connectedSocket socket file descriptor with connected device
/// \param connectedDevice connected device that will receive messages
void communication_transmitter_worker(int32_t connectedSocket, Device connectedDevice);

#endif //FINAL_COMMUNICATION_H
