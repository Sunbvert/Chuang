
#include <stdio.h>

extern "C" {
    #include <amqp.h>
    #include <amqp_framing.h>
    #include <amqp_tcp_socket.h>
    #include <stdlib.h>
    #include <assert.h>
    #include "utils.h"
}

class RpcServer
{
public:
    RpcServer();
    ~RpcServer();

    static void Run(amqp_connection_state_t conn);
private:
    amqp_connection_state_t m_Connection;

}
