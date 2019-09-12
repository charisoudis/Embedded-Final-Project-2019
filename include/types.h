#ifndef FINAL_TYPES_H
#define FINAL_TYPES_H

#include <stdint.h>
#include "conf.h"

#define error(status, msg) do { errno = status; perror(msg); exit(EXIT_FAILURE); } while (0)

// start: Server.h
typedef uint16_t messages_head_t;
typedef uint8_t devices_head_t;

typedef struct active_devices_queue_t {

    char devices_aem_string[ACTIVE_SOCKET_CONNECTIONS_MAX<<2 + ACTIVE_SOCKET_CONNECTIONS_MAX];

} DevicesQueue;
// end

// start: Utils.h
typedef unsigned long uint64;
typedef unsigned int uint;

typedef struct device_t {
    uint32_t AEM;
} Device;

typedef struct message_t {
    // Fundamental Message fields
    uint32_t sender;                    // ΑΕΜ αποστολέα:       uint32
    uint32_t recipient;                 // ΑΕΜ παραλήπτη:       uint32
    uint64 created_at;                  // Χρόνος δημιουργίας:  uint64 ( Linux timestamp - 10 digits at the time of writing )
    char body[256];                     // Κείμενο μηνύματος:   ASCII[256]

    // Metadata
    uint8_t transmitted;                // If the message was actually transmitted from this device
    char transmitted_device_aem_string[50];   // Devices' AEMs that this message was transmitted to
} Message;

/* char[277] type */
typedef char *MessageSerialized;    // length = 4 + 4 + 10 + 256 = 277 characters

/* pthread function arguments pointer */
typedef struct communication_worker_args_t {

    Device connected_device;
    uint16_t connected_socket_fd;
    uint8_t concurrent;
    uint8_t server;

} CommunicationWorkerArgs;
// end

// start: Log.h
typedef struct messages_stats_t {

    // Total
    uint16_t produced;
    uint16_t received;
    uint16_t transmitted;

    // Time
    float producedDelayAvg;

} MessagesStats;
// end

// start: Client.h

// end

#endif //FINAL_TYPES_H
