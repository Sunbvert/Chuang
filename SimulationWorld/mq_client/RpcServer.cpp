/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Portions created by Alan Antonuk are Copyright (c) 2012-2013
 * Alan Antonuk. All Rights Reserved.
 *
 * Portions created by VMware are Copyright (c) 2007-2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Portions created by Tony Garnock-Jones are Copyright (c) 2009-2010
 * VMware, Inc. and Tony Garnock-Jones. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RpcServer.hpp"

#define SUMMARY_EVERY_US 1000000

RpcServer::RpcServer()
{
    char const *hostname;
    int port, status;
    char const *queuename;
    amqp_socket_t *socket = NULL;
    char const *messagebody;

    hostname = "localhost";
    port = 5672;
    queuename = "ask_cpp";

    m_conn = amqp_new_connection();
    m_channel_id = 2;

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
    amqp_channel_open(m_conn, m_channel_id);
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Opening channel");
    
    amqp_queue_declare(m_conn, m_channel_id, amqp_cstring_bytes(queuename), 0, 0, 0, 1, amqp_empty_table);

    amqp_basic_consume(m_conn, m_channel_id, amqp_cstring_bytes(queuename), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    
    die_on_amqp_error(amqp_get_rpc_reply(m_conn), "Consuming");
}

RpcServer::~RpcServer()
{
    die_on_amqp_error(amqp_channel_close(m_conn, m_channel_id, AMQP_REPLY_SUCCESS),
                    "Closing channel");
    die_on_amqp_error(amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS),
                    "Closing connection");
    die_on_error(amqp_destroy_connection(m_conn), "Ending connection");
}

void RpcServer::Run()
{
    const char *messagebody="body";
    {
        for (;;) {
            amqp_rpc_reply_t res;
            amqp_envelope_t envelope;

            amqp_maybe_release_buffers(m_conn);

            res = amqp_consume_message(m_conn, &envelope, NULL, 0);

            if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            break;
        }
        /*
        Once you've consumed a message and processed it,
        your code should publish to the default exchange (amqp_empty_bytes),
        with a routing key that is specified in the reply_to header in the request message.
        Additionally your code should set the correlation_id header to be the same as what is in the request message.
        */
        printf("||Delivery %u,%d exchange %s  %d routingkey %s consumer_tag %s|| \n",
             (unsigned) envelope.delivery_tag,
             (int) envelope.exchange.len,
            (char *) envelope.exchange.bytes,
             (int) envelope.routing_key.len,
            (char *) envelope.routing_key.bytes,
            (char *) envelope.message.properties.reply_to.bytes);

        printf("----\n");

        amqp_dump(envelope.message.body.bytes, envelope.message.body.len);

        //ALE

        amqp_basic_properties_t props;
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
                       AMQP_BASIC_DELIVERY_MODE_FLAG |
                       AMQP_BASIC_CORRELATION_ID_FLAG;
            props.content_type = amqp_cstring_bytes("text/plain");
            props.delivery_mode = 2; // persistent delivery mode
        //Additionally your code should set the correlation_id header to be the same as what is in the request message.
        //props.correlation_id = amqp_cstring_bytes("1");
            props.correlation_id = envelope.message.properties.correlation_id;

        die_on_error(amqp_basic_publish(m_conn,
                                        m_channel_id,
        //your code should publish to the default exchange (amqp_empty_bytes)
                        amqp_empty_bytes,
        // with a routing key that is specified in the reply_to header in the request message.
                                        amqp_cstring_bytes((char *)envelope.message.properties.reply_to.bytes),
                        0,
                                        0,
                                        &props,
                                        amqp_cstring_bytes(messagebody)),
                     "Publishing");

        printf("MESSAGEBODY:[%s]\n",messagebody);
        //amqp_bytes_free(props.reply_to);


        //END ALE



        amqp_destroy_envelope(&envelope);
        }
    }
}
