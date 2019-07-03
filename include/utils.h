#ifndef FINAL_UTILS_H
#define FINAL_UTILS_H

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define error(status, msg) \
               do { errno = status; perror(msg); exit(EXIT_FAILURE); } while (0)

#ifndef SOCKET_PORT
    #define SOCKET_PORT 2278
#endif
#ifndef COMMUNICATION_WORKERS_MAX
    #define COMMUNICATION_WORKERS_MAX 4
#endif


//------------------------------------------------------------------------------------

typedef unsigned long uint64;

typedef struct device_t {
    uint32_t AEM;
    unsigned char mac[6];           // MAC address of this device ( 6 bytes )
} Device;

typedef struct message_t {
    // Fundamental Message fields
    uint32_t sender;                // ΑΕΜ αποστολέα:       uint32
    uint32_t recipient;             // ΑΕΜ παραλήπτη:       uint32
    uint64 created_at;            // Χρόνος δημιουργίας:  uint64 ( Linux timestamp - 10 digits at the time of writing )
    char body[256];                 // Κείμενο μηνύματος:   ASCII[256]

    // Metadata
    uint8_t transmitted;            // If the message was actually transmitted from this device
    Device transmitted_device;      // Device that this message was transmitted to ( if transmitted from this device )
} Message;

/* char[277] type */
typedef char *MessageSerialized;    // length = 4 + 4 + 10 + 256 = 277 characters

/* pthread function arguments pointer */
typedef struct communication_worker_args_t {

    Device connected_device;
    uint16_t connected_socket_fd;

} CommunicationWorkerArgs;

//------------------------------------------------------------------------------------


/// \brief Handle communication staff with connected device ( POSIX thread compatible function ).
/// \param thread_args pointer to communicate_args_t type
void communication_worker(void *args);

/// \brief Receiver sub-worker of communication worker ( POSIX thread compatible function ).
/// \param thread_args pointer to communicate_args_t type
void communication_receiver_worker(void *args);

/// \brief Transmitter sub-worker of communication worker ( POSIX thread compatible function ).
/// \param receiver connected device that will receive messages
/// \param socket_fd socket file descriptor with connected device
void communication_transmitter_worker(Device receiver, int socket_fd);

/// \brief Un-serializes message-as-a-string, re-creating initial message.
/// \param glue the connective character(s); acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
Message explode(const char *glue, MessageSerialized messageSerialized);

/// \brief Generates a new message from this device towards $recipient with $body as content.
/// \param recipient message's recipient
/// \param body message's body
/// \return newly generated message of type message_t
Message generateMessage(uint32_t recipient, const char * body);

/// \brief Generates a new random message composed of:
///     - random recipient  ( 4 randomly generated digits: {1-9}{0-9}{0-9}{0-9} )
///     - random body       ( 256 randomly generated ascii characters )
///     - CLIENT_AEM as sender
///     - creation time_of_day as created_at timestamp
/// \return newly generated message of type message_t
Message generateRandomMessage();

/// Convert HEX string to an array of bytes representing MAC address.
/// \param hex HEX string ( successive bytes should be glued together using ':' )
/// \param mac MAC address as array of bytes ( 'byte' is 'unsigned char' in C )
void hex2mac(const char * hex, unsigned char * mac);

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character(s); to be placed between successive message fields
/// \param message the message to be serialized
/// \param messageSerialized a string containing all message fields glued together using $glue
void implode(const char *glue, Message message, MessageSerialized messageSerialized);

/// \brief Log ( to stdout ) message's fields.
/// \param message
/// \param metadata show/hide metadata information from message
void inspect(Message message, uint8_t metadata);

/// \brief Extracts AEM from given IPv4 address.
/// \param ip string ( resulting from inet_ntop() )
/// \return aem uint32_t
uint32_t ip2aem(const char *ip);

/// \brief Check if two devices have exactly the same values in ALL of their fields.
/// \param device1
/// \param device2
/// \return
uint8_t isDeviceEqual(Device device1, Device device2);

/// \brief Check if two messages have exactly the same values in ALL of their fields ( metadata excluded ).
/// \param message1
/// \param message2
/// \return
uint8_t isMessageEqual(Message message1, Message message2);

/// Convert MAC address from byte array to string ( adding ':' between successive bytes )
/// \param mac mac address as array of bytes ( 'byte' is 'unsigned char' in C )
/// \param hex pointer to the HEX string of the MAC address
void mac2hex(const unsigned char *mac, char *hex);

/// \brief Tries to connect via socket to given IP address & port.
/// \param ip the IP address to open socket to
/// \return -1 on error, opened socket's file descriptor on success
int socket_connect(const char * ip);

/// \brief Convert given UNIX timestamp to a formatted datetime string with given $format.
/// \param timestamp UNIX timestamp ( uint64 )
/// \param format strftime-compatible format
/// \param string the resulting datetime string
void timestamp2ftime( uint64 timestamp, const char *format, char *string );

#endif //FINAL_UTILS_H
