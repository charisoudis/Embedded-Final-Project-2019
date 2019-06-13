#ifndef FINAL_UTILS_H
#define FINAL_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct message_t {
    uint32_t sender;                // ΑΕΜ αποστολέα:       uint32
    uint32_t recipient;             // ΑΕΜ παραλήπτη:       uint32
    uint64_t created_at;            // Χρόνος δημιουργίας:  uint64 ( Linux timestamp - 10 digits at the time of writing )
    char body[256];                 // Κείμενο μηνύματος:   ASCII[256]
} Message;

/* char[277] type */
typedef char *MessageSerialized;    // length = 4 + 4 + 10 + 256 = 277 characters

/// \brief Un-serializes message-as-a-string and recreates message struct.
/// \param glue the connective character; acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
Message explode(char glue, MessageSerialized messageSerialized);

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character; to be placed between successive message fields
/// \param message the message to be serialized
/// \return a string containing all message fields glued together using $glue
MessageSerialized implode(char glue, Message message);

/// \brief Log ( to stdout ) message's fields.
/// \param message
void inspect(Message message);

/// \brief Check if two messages have exactly the same values in ALL of their fields.
/// \param message1
/// \param message2
/// \return
bool isEqual(Message message1, Message message2);

#endif //FINAL_UTILS_H
