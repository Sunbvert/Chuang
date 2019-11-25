//
//  MqRpc.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/24.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef MqRpc_hpp
#define MqRpc_hpp

#include <stdio.h>

extern "C" {
    #include <amqp.h>
    #include <amqp_framing.h>
    #include <amqp_tcp_socket.h>
    #include <stdlib.h>
    #include <assert.h>
    #include "utils.h"
}

#include "json.hpp"

using json = nlohmann::json;

class MqRpc
{
public:
    MqRpc();
    ~MqRpc();
    
    void EstablishConnection();
    void ServerRun(json &j);
    void ClientCall();
    
private:
    amqp_connection_state_t m_conn;
    amqp_channel_t m_server_channel;
    amqp_channel_t m_client_channel;
    amqp_bytes_t m_reply_queue;
    
    bool connected;
    
    void BeginAsServer();
    void BeginAsClient();
};

#endif /* MqRpc_hpp */
