import pika
import uuid
import json

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

class RpcClient(object):

    def __init__(self):
        self.connection = pika.BlockingConnection(
            pika.ConnectionParameters(host='localhost'))

        self.channel = self.connection.channel()

        result = self.channel.queue_declare(queue='', exclusive=True)
        self.callback_queue = result.method.queue

        self.channel.basic_consume(
            queue=self.callback_queue,
            on_message_callback=self.on_response,
            auto_ack=True)

    def on_response(self, ch, method, props, body):
        if self.corr_id == props.correlation_id:
            self.response = body

    def call(self, j):
        self.response = None
        self.corr_id = str(uuid.uuid4())
        self.channel.basic_publish(
            exchange='',
            routing_key='ask_cpp',
            properties=pika.BasicProperties(
                reply_to=self.callback_queue,
                correlation_id=self.corr_id,
            ),
            body = j)
        while self.response is None:
            self.connection.process_data_events()
        data = bytes(self.response).decode('utf8');
        json_response = json.loads(data)
        return json_response


class Dispatcher(object):
    def __init__(self):
        self.rpc_client = RpcClient();

    def make(self, num_envs):
        data = {
            'call': 'make',
            'num_envs': num_envs
        }
        json_str = json.dumps(data);
        
        response = self.rpc_client.call(json_str)
        if (response['status']):
            self.observation_space = response['observation_space']
            self.action_space = response['action_space']
            return True
        return False

    def step(self, actions):
        # TODO: check action dimensions
        data = {
            'call': 'step',
            'action': actions
        }
        json_str = json.dumps(data)

        response = self.rpc_client.call(json_str)
        return response['observation'], response['reward'], response['done'], ""

    def reset(self):
        data = {
            'call': 'reset'
        }
        json_str = json.dumps(data)

        response = self.rpc_client.call(json_str)
        return response['observation']