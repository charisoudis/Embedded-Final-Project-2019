#ifndef FINAL_UTILS_H
#define FINAL_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct device_t {
    uint32_t AEM;
    unsigned char mac[6];           // MAC address of this device ( 6 bytes )
} Device;

typedef struct message_t {
    // Fundamental Message fields
    uint32_t sender;                // ΑΕΜ αποστολέα:       uint32
    uint32_t recipient;             // ΑΕΜ παραλήπτη:       uint32
    uint64_t created_at;            // Χρόνος δημιουργίας:  uint64 ( Linux timestamp - 10 digits at the time of writing )
    char body[256];                 // Κείμενο μηνύματος:   ASCII[256]

    // Metadata
    bool transmitted;               // If the message was actually transmitted from this device
    Device transmitted_device;      // Device that this message was transmitted to ( if transmitted from this device )
} Message;

/* char[277] type */
typedef char *MessageSerialized;    // length = 4 + 4 + 10 + 256 = 277 characters

/// \brief Un-serializes message-as-a-string, re-creating initial message.
/// \param glue the connective character(s); acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
Message explode(const char *glue, MessageSerialized messageSerialized);

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
void inspect(Message message, bool metadata);

/// \brief Check if two messages have exactly the same values in ALL of their fields ( metadata excluded ).
/// \param message1
/// \param message2
/// \return
bool isEqual(Message message1, Message message2);

/// Convert MAC address from byte array to string ( adding ':' between successive bytes )
/// \param mac mac address as array of bytes ( 'byte' is 'unsigned char' in C )
/// \param hex pointer to the HEX string of the MAC address
void mac2hex(const unsigned char *mac, char *hex);

/// \brief Convert given UNIX timestamp to a formatted datetime string with given $format.
/// \param timestamp UNIX timestamp ( uint64_t )
/// \param format strftime-compatible format
/// \param string the resulting datetime string
void timestamp2ftime( uint64_t timestamp, const char *format, char *string );

#endif //FINAL_UTILS_H
