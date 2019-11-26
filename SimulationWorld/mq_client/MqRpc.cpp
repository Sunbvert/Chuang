//
//  MqRpc.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/24.
//  Copyright © 2019 孙川. All rights reserved.
//
#include <string.h>
#include <iostream>

#include "MqRpc.hpp"


bool MqRpc::connected = false;
amqp_connection_state_t MqRpc::m_conn;
amqp_channel_t MqRpc::m_server_channel = 2;
amqp_channel_t MqRpc::m_client_channel = 1;
amqp_bytes_t MqRpc::m_reply_queue;
std::thread MqRpc::m_server_thread;


void MqRpc::EstablishConnection()
{
    char const *hostname;
    int port, status;
    char const *server_queue;
    char const *client_queue;
    amqp_socket_t *socket = NULL;

    hostname = "localhost";
    port = 5672;
    server_queue = "ask_cpp";
    client_queue = "ask_python";
    status = 0;
    
    m_conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(m_conn);
    if (!socket)
    {
        die("creating TCP socket");
    }

    status = amqp_socket_open(socket, hostname, port);
    if (status)
    {
        die("opening TCP socket");
    }

    die_on_amqp_error(amqp_login(m_conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest"),
                    "Logging in");
    
    //BeginAsClient();
    BeginAsServer();
}

void MqRpc::BeginAsServer()
{
    char const *server_queue = "ask_cpp";
    amqp_channel_open(m_conn, m_server_channel);
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Opening channel");
    amqp_queue_declare(m_conn, m_server_channel, amqp_cstring_bytes(server_queue), 0, 0, 0, 1, amqp_empty_table);

    amqp_basic_consume(m_conn, m_server_channel, amqp_cstring_bytes(server_queue), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Consuming");
}

void MqRpc::BeginAsClient()
{
    amqp_channel_open(m_conn, m_client_channel);
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Opening channel");
    amqp_queue_declare_ok_t *r = amqp_queue_declare(
        m_conn, m_client_channel, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Declaring queue");
    m_reply_queue = amqp_bytes_malloc_dup(r->queue);
    if (m_reply_queue.bytes == NULL) {
      fprintf(stderr, "Out of memory while copying queue name");
    }
}

void MqRpc::ServerRun(MqConnCallback &callbackContext)
{
    if (!connected)
    {
        m_server_thread = std::thread(ListenAndRecieveData, std::ref(callbackContext));
    }
    else
    {
        std::cout << "warnning: server thread already running!" << std::endl;
    }
}

void MqRpc::ListenAndRecieveData(MqConnCallback &callbackContext)
{
    
    {
        connected = true;
        for (;;) {
            amqp_rpc_reply_t res;
            amqp_envelope_t envelope;

            amqp_maybe_release_buffers(m_conn);
            res = amqp_consume_message(m_conn, &envelope, NULL, 0);
            if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            break;
        }
        
        unsigned char *recieved = (unsigned char *)envelope.message.body.bytes;
        std::string s( reinterpret_cast<char const* >(recieved), envelope.message.body.len);
        
        json receivedJson = json::parse(s);

        json j = callbackContext.OnDataRecieve(receivedJson);
        //ALE
            
        std::string send = j.dump();
        amqp_bytes_t_ body_bytes = amqp_cstring_bytes(send.c_str());

        amqp_basic_properties_t props;
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
                       AMQP_BASIC_DELIVERY_MODE_FLAG |
                       AMQP_BASIC_CORRELATION_ID_FLAG;
            props.content_type = amqp_cstring_bytes("application/json");
            props.delivery_mode = 2; // persistent delivery mode
        //Additionally your code should set the correlation_id header to be the same as what is in the request message.
        //props.correlation_id = amqp_cstring_bytes("1");
            props.correlation_id = envelope.message.properties.correlation_id;

        die_on_error(amqp_basic_publish(m_conn,
                                        m_server_channel,
        //your code should publish to the default exchange (amqp_empty_bytes)
                        amqp_empty_bytes,
        // with a routing key that is specified in the reply_to header in the request message.
                                        amqp_cstring_bytes((char *)envelope.message.properties.reply_to.bytes),
                        0,
                                        0,
                                        &props,
                                        body_bytes),
                     "Publishing");
        
        //amqp_bytes_free(props.reply_to);


        //END ALE
        amqp_destroy_envelope(&envelope);
        }
    }
}

void MqRpc::ClientCall()
{
    
    json j = R"({"compact": true, "schema": 0})"_json;
    std::vector<std::uint8_t> v_bson = json::to_bson(j);
    
    /*
       send the message
    */

    {
      /*
        set properties
      */
      amqp_basic_properties_t props;
      props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
                     AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_REPLY_TO_FLAG |
                     AMQP_BASIC_CORRELATION_ID_FLAG;
      props.content_type = amqp_cstring_bytes("application/json");
        props.delivery_mode = 2; /* persistent delivery mode */
      props.reply_to = amqp_bytes_malloc_dup(m_reply_queue);
      if (props.reply_to.bytes == NULL) {
        fprintf(stderr, "Out of memory while copying queue name");
      }
      props.correlation_id = amqp_cstring_bytes("1");

      /*
        publish
      */
      die_on_error(amqp_basic_publish(m_conn, 1, amqp_cstring_bytes(""),
                                      amqp_cstring_bytes("ask_python"), 0, 0,
                                      &props, amqp_cstring_bytes("m_message_body")),
                   "Publishing");

      amqp_bytes_free(props.reply_to);
    }
    
    /*
      wait an answer
    */

    {
      amqp_basic_consume(m_conn, 1, m_reply_queue, amqp_empty_bytes, 0, 1, 0,
                         amqp_empty_table);
      die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Consuming");
      amqp_bytes_free(m_reply_queue);

      {
        amqp_frame_t frame;
        int result;

        amqp_basic_deliver_t *d;
        amqp_basic_properties_t *p;
        size_t body_target;
        size_t body_received;

        for (;;) {
          amqp_maybe_release_buffers(m_conn);
          result = amqp_simple_wait_frame(m_conn, &frame);
          printf("Result: %d\n", result);
          if (result < 0) {
            break;
          }

          printf("Frame type: %u channel: %u\n", frame.frame_type, frame.channel);
          if (frame.frame_type != AMQP_FRAME_METHOD) {
            continue;
          }

          printf("Method: %s\n", amqp_method_name(frame.payload.method.id));
          if (frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD) {
            continue;
          }

          d = (amqp_basic_deliver_t *)frame.payload.method.decoded;
          printf("Delivery: %u exchange: %.*s routingkey: %.*s\n",
                 (unsigned)d->delivery_tag, (int)d->exchange.len,
                 (char *)d->exchange.bytes, (int)d->routing_key.len,
                 (char *)d->routing_key.bytes);

          result = amqp_simple_wait_frame(m_conn, &frame);
          if (result < 0) {
            break;
          }

          if (frame.frame_type != AMQP_FRAME_HEADER) {
            fprintf(stderr, "Expected header!");
            abort();
          }
          p = (amqp_basic_properties_t *)frame.payload.properties.decoded;
          if (p->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
            printf("Content-type: %.*s\n", (int)p->content_type.len,
                   (char *)p->content_type.bytes);
          }
          printf("----\n");

          body_target = (size_t)frame.payload.properties.body_size;
          body_received = 0;

          while (body_received < body_target) {
            result = amqp_simple_wait_frame(m_conn, &frame);
            if (result < 0) {
              break;
            }

            if (frame.frame_type != AMQP_FRAME_BODY) {
              fprintf(stderr, "Expected body!");
              abort();
            }

            body_received += frame.payload.body_fragment.len;
            assert(body_received <= body_target);

            amqp_dump(frame.payload.body_fragment.bytes,
                      frame.payload.body_fragment.len);
          }

          if (body_received != body_target) {
            /* Can only happen when amqp_simple_wait_frame returns <= 0 */
            /* We break here to close the connection */
            break;
          }
            
          /* everything was fine, we can quit now because we received the reply */
          break;
        }
      }
    }
}
