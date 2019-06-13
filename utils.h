//
// Created by achariso on 13/6/2019.
//

#ifndef FINAL_UTILS_H
#define FINAL_UTILS_H

#include <tiff.h>

typedef struct message_t {
    uint32 sender;                  // ΑΕΜ αποστολέα:       uint32
    uint32 receiver;                // ΑΕΜ παραλήπτη:       uint32
    uint64 created_at;              // Χρόνος δημιουργίας:  uint64 ( Linux timestamp - 10 digits at the time of writing )
    unsigned char body[256];        // Κείμενο μηνύματος:   ASCII[256]
} Message;

typedef unsigned char *MessageSerialized;    // length = 4 + 4 + 10 + 256 = 274 characters

/// \brief Serializes a message ( of message_t type ) into a 274-characters string.
/// \param glue the connective character; to be placed between successive message fields
/// \param message the message to be serialized
/// \return a string containing all message fields glued together using $glue
MessageSerialized implode(char glue, Message message);

/// \brief Un-serializes message-as-a-string and recreates message struct.
/// \param glue the connective character; acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
Message explode(char glue, MessageSerialized messageSerialized);

#endif //FINAL_UTILS_H
