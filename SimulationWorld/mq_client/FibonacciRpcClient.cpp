//
//  FibonacciRpcClient.cpp
//  amqpTest
//
//  Created by 孙川 on 2019/11/6.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "FibonacciRpcClient.hpp"

FibonacciRpcClient::FibonacciRpcClient()
{
    m_Exchange = "";
    m_Routingkey = "rpc_queue";
    m_Messagebody = "Hello RPC!";
    
    /*
       establish a channel that is used to connect RabbitMQ server
    */
    m_Connection = amqp_new_connection();
    
    int status = 0;
    int port = 5672;
    const char *hostname = "localhost";
    amqp_socket_t *socket = NULL;
    socket = amqp_tcp_socket_new(m_Connection);
    if (!socket) {
      die("creating TCP socket");
    }

    status = amqp_socket_open(socket, hostname, port);
    if (status) {
      die("opening TCP socket");
    }

    die_on_amqp_error(amqp_login(m_Connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                 "guest", "guest"),
                      "Logging in");
    amqp_channel_open(m_Connection, 1);
    die_on_amqp_error(amqp_get_rpc_reply(m_Connection), "Opening channel");
    
    /*
       create private reply_to queue
    */

    {
      amqp_queue_declare_ok_t *r = amqp_queue_declare(
          m_Connection, 1, amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);
      die_on_amqp_error(amqp_get_rpc_reply(m_Connection), "Declaring queue");
      m_ReplyToQueue = amqp_bytes_malloc_dup(r->queue);
      if (m_ReplyToQueue.bytes == NULL) {
        fprintf(stderr, "Out of memory while copying queue name");
      }
    }
}

FibonacciRpcClient::~FibonacciRpcClient()
{
    die_on_amqp_error(amqp_channel_close(m_Connection, 1, AMQP_REPLY_SUCCESS),
                      "Closing channel");
    die_on_amqp_error(amqp_connection_close(m_Connection, AMQP_REPLY_SUCCESS),
                      "Closing connection");
    die_on_error(amqp_destroy_connection(m_Connection), "Ending connection");
}

void FibonacciRpcClient::Call()
{
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
      props.content_type = amqp_cstring_bytes("text/plain");
      props.delivery_mode = 2; /* persistent delivery mode */
      props.reply_to = amqp_bytes_malloc_dup(m_ReplyToQueue);
      if (props.reply_to.bytes == NULL) {
        fprintf(stderr, "Out of memory while copying queue name");
      }
      props.correlation_id = amqp_cstring_bytes("1");

      /*
        publish
      */
      die_on_error(amqp_basic_publish(m_Connection, 1, amqp_cstring_bytes(m_Exchange),
                                      amqp_cstring_bytes(m_Routingkey), 0, 0,
                                      &props, amqp_cstring_bytes(m_Messagebody)),
                   "Publishing");

      amqp_bytes_free(props.reply_to);
    }
    
    /*
      wait an answer
    */

    {
      amqp_basic_consume(m_Connection, 1, m_ReplyToQueue, amqp_empty_bytes, 0, 1, 0,
                         amqp_empty_table);
      die_on_amqp_error(amqp_get_rpc_reply(m_Connection), "Consuming");
      amqp_bytes_free(m_ReplyToQueue);

      {
        amqp_frame_t frame;
        int result;

        amqp_basic_deliver_t *d;
        amqp_basic_properties_t *p;
        size_t body_target;
        size_t body_received;

        for (;;) {
          amqp_maybe_release_buffers(m_Connection);
          result = amqp_simple_wait_frame(m_Connection, &frame);
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

          result = amqp_simple_wait_frame(m_Connection, &frame);
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
            result = amqp_simple_wait_frame(m_Connection, &frame);
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
