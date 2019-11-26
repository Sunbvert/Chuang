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
#include <thread>

extern "C" {
    #include <amqp.h>
    #include <amqp_framing.h>
    #include <amqp_tcp_socket.h>
    #include <stdlib.h>
    #include <assert.h>
    #include "utils.h"
}

#include "json.hpp"
#include "MqConnCallback.hpp"

using json = nlohmann::json;

class MqRpc
{
public:
    static void EstablishConnection();
    static void ServerRun(MqConnCallback &callbackContext);
    static void ClientCall();
private:
    MqRpc() {};
    
    static void ListenAndRecieveData(MqConnCallback &callbackContext);
    
    static amqp_connection_state_t m_conn;
    static amqp_channel_t m_server_channel;
    static amqp_channel_t m_client_channel;
    static amqp_bytes_t m_reply_queue;
    
    static bool connected;
    static std::thread m_server_thread;
    
    static void BeginAsServer();
    static void BeginAsClient();
};

#endif /* MqRpc_hpp */
