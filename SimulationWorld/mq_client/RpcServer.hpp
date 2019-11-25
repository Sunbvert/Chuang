
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

    void Run();
private:
    amqp_connection_state_t m_conn;
    amqp_channel_t m_channel_id;
};
