#ifndef FINAL_SERVER_H
#define FINAL_SERVER_H

#include "types.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

/// \brief Check if $device exists in $activeDevices FIFO queue.
/// \param device
/// \return uint8 0 if FALSE, 1 if TRUE
uint8_t devices_exists(Device device);

/// \brief Push $device to $activeDevices FIFO queue.
/// \param device
void devices_push(Device device);

/// \brief Remove $device from $activeDevices FIFO queue.
/// \param device
void devices_remove(Device device);

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message message);

/// \brief Main server loop. Calls communication_thread() on each new connection.
void listening_worker();

#endif //FINAL_SERVER_H
