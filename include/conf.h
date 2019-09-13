#ifndef FINAL_CONF_H
#define FINAL_CONF_H

#include <stdint.h>
#include <stdbool.h>

// start: Client.h
#ifndef CLIENT_AEM_RANGE
    #define CLIENT_AEM_RANGE_MIN 8000
    #define CLIENT_AEM_RANGE_MAX 9050
#endif

#ifndef CLIENT_AEM_LIST
    // Sorted list of AEMs
    static int CLIENT_AEM_LIST[] = {
            0001,
            8000,
            8600,
            8723,
            8859,
            8888,
            9005,
            9026,
            9999
    };

    #define CLIENT_AEM_LIST_LENGTH ( uint32_t )( sizeof( CLIENT_AEM_LIST ) / sizeof( int ) )

    // Active flag for each AEM
    static bool CLIENT_AEM_ACTIVE_LIST[CLIENT_AEM_LIST_LENGTH] = {false};
#endif

#ifndef CLIENT_AEM_SOURCE
    #define CLIENT_AEM_SOURCE_RANGE 0
    #define CLIENT_AEM_SOURCE_LIST 1
    #define CLIENT_AEM_SOURCE CLIENT_AEM_SOURCE_LIST
#endif

#ifndef PRODUCER_DELAY_RANGE    // in minutes
    #define PRODUCER_DELAY_RANGE_MIN 1
    #define PRODUCER_DELAY_RANGE_MAX 5
#endif

#ifndef MESSAGE_SERIALIZED_LEN
    #define MESSAGE_BODY_LEN 256
    #define MESSAGE_SERIALIZED_LEN 277  // length = 4 + 4 + 10 + 256 = 277 characters
#endif
// end

// start: Server.h
#ifndef MESSAGES_PUSH_OVERRIDE_POLICY
    #define MESSAGES_PUSH_OVERRIDE_SENT_ONLY 0
    #define MESSAGES_PUSH_OVERRIDE_BLIND 1
    #define MESSAGES_PUSH_OVERRIDE_POLICY MESSAGES_PUSH_OVERRIDE_SENT_ONLY
#endif

#ifndef MESSAGES_SIZE
    #define MESSAGES_SIZE 2000
#endif
// end

// start: Utils.h
#ifndef SOCKET_PORT
    #define SOCKET_PORT 2278
#endif

#ifndef ACTIVE_SOCKET_CONNECTIONS_MAX
    #define ACTIVE_SOCKET_CONNECTIONS_MAX 4     // >=2: 1 ( server ) + 1 ( client ) + ...( other concurrent sockets )...
#endif
#ifndef COMMUNICATION_WORKERS_MAX
    #define COMMUNICATION_WORKERS_MAX (ACTIVE_SOCKET_CONNECTIONS_MAX - 2)   // 0 == serial socket communications
#endif

#ifndef STRSEP_BASE_10
    #define STRSEP_BASE_10 10
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
