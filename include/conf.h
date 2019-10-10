#ifndef FINAL_CONF_H
#define FINAL_CONF_H

#include <stdint.h>
#include <stdbool.h>

// start: main
#ifndef MAX_EXECUTION_TIME
    #define MAX_EXECUTION_TIME 7200               // 2 hours
#endif
// end

// start: Communication.h
#ifndef SYNC_DATETIME
    #define SYNC_DATETIME 0                       // flag to sync date time
    #define SETUP_DATETIME_AEM 0001               // Device with AEM = 0001 will setup datetime with all connected devices
    #define SETUP_DATETIME_TIMEOUT 10             // secs
#endif
// end

// start: Client.h
#ifndef CLIENT_AEM_RANGE
    #define CLIENT_AEM_RANGE_MIN 8000
    #define CLIENT_AEM_RANGE_MAX 9050

    #define CLIENT_AEM_RANGE_LENGTH (CLIENT_AEM_RANGE_MAX - CLIENT_AEM_RANGE_MIN + 1)
#endif

#ifndef CLIENT_AEM_LIST_LENGTH
    // Sorted list of AEMs
    static const uint32_t CLIENT_AEM_LIST[] = {
        0001, 7051, 8001, 8011, 8032, 8600, 8723, 8859,
        8869, 8888, 8998, 8999, 9005, 9026, 9028, 9999
    };

    #define CLIENT_AEM_LIST_LENGTH ( uint32_t )( sizeof( CLIENT_AEM_LIST ) / sizeof( int ) )
#endif

#ifndef CLIENT_AEM_SOURCE
    #define CLIENT_AEM_SOURCE "list"    // "list", "range"
#endif

#ifndef MAX_CONNECTIONS_WITH_SAME_CLIENT
    #define MAX_CONNECTIONS_WITH_SAME_CLIENT 1000
#endif

#ifndef PRODUCER_DELAY_RANGE    // in seconds
    #define PRODUCER_DELAY_RANGE_MIN 60     // 1 min
    #define PRODUCER_DELAY_RANGE_MAX 300    // 5 min
#endif

#ifndef MESSAGE_SERIALIZED_LEN
    #define MESSAGE_BODY_LEN 256
    #define MESSAGE_SERIALIZED_LEN 277  // length = 4 + 4 + 10 + 256 = 277 characters
#endif
// end

// start: Server.h
#ifndef MESSAGES_PUSH_OVERRIDE_POLICY
    #define MESSAGES_PUSH_OVERRIDE_POLICY "blind"   // "sent_only", "blind"
#endif

#ifndef MESSAGES_SIZE
    #define MESSAGES_SIZE 2000
#endif

#ifndef INBOX_SIZE
    #define INBOX_SIZE 1000
#endif

#ifndef SOCKET_LISTEN_QUEUE_LEN
    #define SOCKET_LISTEN_QUEUE_LEN 5
#endif
// end

// start: Utils.h
#ifndef SOCKET_PORT
    #define SOCKET_PORT 2278
#endif

#ifndef ACTIVE_SOCKET_CONNECTIONS_MAX
    #define ACTIVE_SOCKET_CONNECTIONS_MAX 2     // >=2: 1 ( server ) + 1 ( client ) + ...( other concurrent sockets )...
#endif
#ifndef COMMUNICATION_WORKERS_MAX
    #define COMMUNICATION_WORKERS_MAX (ACTIVE_SOCKET_CONNECTIONS_MAX - 2)   // 0 == serial socket communications
#endif

#ifndef STRSEP_BASE_10
    #define STRSEP_BASE_10 10
#endif

#ifndef STRFTIME_STR_LEN
    #define STRFTIME_STR_LEN 50
#endif

#ifndef MESSAGE_BODY_ASCII_MIN
    #define MESSAGE_BODY_ASCII_MIN 32
    #define MESSAGE_BODY_ASCII_MAX 95
#endif
// end

// start: Log.h
#ifndef ALSO_LOG_TO_STDOUT
    #define ALSO_LOG_TO_STDOUT 1
#endif

#ifndef LOG_MESSAGE_MAX_LEN
    #define LOG_MESSAGE_MAX_LEN 512
#endif
// end

#endif //FINAL_CONF_H
