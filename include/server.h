#ifndef FINAL_SERVER_H
#define FINAL_SERVER_H

#include "utils.h"
#include "log.h"

#ifndef MESSAGES_PUSH_OVERRIDE_POLICY
    #define MESSAGES_PUSH_OVERRIDE_SENT_ONLY 0
    #define MESSAGES_PUSH_OVERRIDE_BLIND 1
    #define MESSAGES_PUSH_OVERRIDE_POLICY MESSAGES_PUSH_OVERRIDE_SENT_ONLY
#endif

#ifndef MESSAGES_SIZE
    #define MESSAGES_SIZE 2000
#endif

typedef uint16_t messages_head_t;
typedef uint8_t devices_head_t;

typedef struct active_devices_queue_t {

    Device devices[ACTIVE_SOCKET_CONNECTIONS_MAX];
    devices_head_t head;
    devices_head_t tail;

} ActiveDevicesQueue;


/// Check if $device exists $activeDevices FIFO queue.
/// \param device
uint8_t devices_exists(Device device);

/// Push $device to $activeDevices FIFO queue.
/// \param device
void devices_push(Device device);

/// Remove $device from $activeDevices FIFO queue.
/// \param device
void devices_remove(Device device);

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message message);

/// \brief Main server loop. Calls communication_thread() on each new connection.
void listening_worker();

#endif //FINAL_SERVER_H
