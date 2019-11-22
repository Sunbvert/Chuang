import pika

class RpcServer:

    def __init__(self):
        connection = pika.BlockingConnection(
            pika.ConnectionParameters(host='localhost'))

        self.channel = connection.channel()

        self.channel.queue_declare(queue='rpc_queue')

        self.channel.basic_qos(prefetch_count=1)
        self.channel.basic_consume(queue='rpc_queue', on_message_callback=self.on_request)

        print(" [x] Awaiting RPC requests")
        self.channel.start_consuming()

    def on_request(ch, method, props, body):
        n = body

        response = str(body) + "recieved!"

        ch.basic_publish(exchange='',
                         routing_key=props.reply_to,
                         properties=pika.BasicProperties(correlation_id = \
                                                             props.correlation_id),
                         body=str(response))
        ch.basic_ack(delivery_tag=method.delivery_tag)