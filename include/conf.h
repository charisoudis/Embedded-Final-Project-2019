#ifndef FINAL_CONF_H
#define FINAL_CONF_H

// start: Client.h
#ifndef CLIENT_AEM
    #define CLIENT_AEM 9026
#endif

#ifndef CLIENT_AEM_RANGE
    #define CLIENT_AEM_RANGE_MIN 8000
    #define CLIENT_AEM_RANGE_MAX 9050
#endif

#ifndef PRODUCER_DELAY_RANGE    // in minutes
    #define PRODUCER_DELAY_RANGE_MIN 1
    #define PRODUCER_DELAY_RANGE_MAX 5
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
// end

#endif //FINAL_CONF_H
