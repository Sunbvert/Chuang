//
//  FibonacciRpcClient.hpp
//  amqpTest
//
//  Created by 孙川 on 2019/11/6.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef FibonacciRpcClient_hpp
#define FibonacciRpcClient_hpp

#include <stdio.h>

extern "C" {
    #include <amqp.h>
    #include <amqp_framing.h>
    #include <amqp_tcp_socket.h>
    #include <stdlib.h>
    #include <assert.h>
    #include "utils.h"
}

class FibonacciRpcClient
{
public:
    FibonacciRpcClient();
    ~FibonacciRpcClient();
    
    void OnResponse();
    void Call();
private:
    amqp_connection_state_t m_Connection;
    amqp_bytes_t m_ReplyToQueue;
    int m_ChannelId;
    const char *m_Exchange;
    const char *m_Routingkey;
    const char *m_Messagebody;
};

#endif /* FibonacciRpcClient_hpp */
