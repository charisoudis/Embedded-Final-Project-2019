#ifndef FINAL_UTILS_H
#define FINAL_UTILS_H

#include "types.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// \brief Constructs IPv4 address from given AEM.
/// \param aem uint32_t
/// \return ip string
const char* aem2ip(uint32_t aem);

/// \brief Perform binary search in $haystack array for $needle and return index of $needle or -1.
/// \param haystack
/// \param N size of $haystack
/// \param needle
/// \return index [0, N-1] if found, -1 else
int32_t binary_search_index(const uint32_t *haystack, size_t N, uint32_t needle);

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

/// \brief Un-serializes message-as-a-string, re-creating initial message.
/// \param message the result message ( passes as a pointer )
/// \param glue the connective character(s); acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
void explode(Message *message, const char * glue, char * messageSerialized);

/// \brief Generates a new message from this client towards $recipient with $body as content.
/// \param message result message ( passed as pointer )
/// \param recipient message's recipient
/// \param body message's body
void generateMessage(Message *message, uint32_t recipient, const char * body);

/// \brief Generates a new random message composed of:
///     - random recipient  ( 4 randomly generated digits: {1-9}{0-9}{0-9}{0-9} )
///     - random body       ( 256 randomly generated ascii characters )
///     - CLIENT_AEM as sender
///     - creation time_of_day as created_at timestamp
/// \param message result message ( passed as pointer )
void generateRandomMessage(Message *message);

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character(s); to be placed between successive message fields
/// \param message the message to be serialized
/// \param messageSerialized a string containing all message fields glued together using $glue
void implode(const char *glue, Message message, char *messageSerialized);

/// \brief Log ( to stdout ) message's fields.
/// \param message
/// \param metadata show/hide metadata information from message
/// \param fp file pointer to where to output inspection
void inspect(Message message, bool metadata, FILE *fp);

/// \brief Inspect all messages in $messages buffer.
/// \param inspect_each
void inspect_messages(bool inspect_each);

/// \brief Extracts AEM from given IPv4 address.
/// \param ip string ( resulting from inet_ntop() )
/// \return aem uint32_t
uint32_t ip2aem(const char *ip);

/// \brief Check if two messages have exactly the same values in ALL of their fields ( metadata excluded ).
/// \param message1
/// \param message2
/// \return
bool isMessageEqual(Message message1, Message message2);

/// \brief Tries to connect via socket to given IP address & port.
/// \param ip the IP address to open socket to
/// \return -1 on error, opened socket's file descriptor on success
int socket_connect(const char * ip);

///// \brief Append $new string to $base string ( supp. that $base has been pre-malloc'ed to fit both ).
///// \param base
///// \param new
//void str_append( char *base, char *new );
//
///// \brief Append $aem to $base string ( supp. that $base has been pre-malloc'ed to fit both ).
///// \param base
///// \param aem
///// \param sep separator character between successive AEMs
//void str_append_aem( char *base, uint32_t aem, const char *sep );
//
///// \brief Check if $needle exists ( is substring ) in $haystack.
///// \param haystack
///// \param needle
///// \return 0 ( false ) / 1 ( true )
//bool str_exists(const char *haystack, const char *needle);
//
///// Check if $aem exists in $haystack string.
///// \param haystack
///// \param aem
///// \return
//bool str_exists_aem(const char *haystack, uint32_t aem);
//
///// \brief Removes $toRemove substring from $str haystack.
///// \param str
///// \param toRemove
//void str_remove(char *str, const char *toRemove);

/// \brief Convert given UNIX timestamp to a formatted datetime string with given $format.
/// \param timestamp UNIX timestamp ( uint64 )
/// \param format strftime-compatible format
/// \param string the resulting datetime string
void timestamp2ftime(uint64_t timestamp, const char *format, char *string );

#endif //FINAL_UTILS_H
