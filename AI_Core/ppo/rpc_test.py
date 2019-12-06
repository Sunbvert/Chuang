from lib.rabbitmq import RpcClient, Dispatcher
import json

def test_step(dispatcher):
    action = [0.0, 0.0, 0.0, 1.0, 2.0, 3.0]
    observation, reward, done, _ = dispatcher.step(action)
    print("observation", observation)
    print("reward", reward)
    print("done", done)

if __name__ == "__main__":
    # fibonacci_rpc = RpcClient()

    # print(" [x] Requesting fib(30)")
    # data = [ { 'a' : 1, 'b' : 2, 'c' : 3, 'd' : 4, 'e' : 5 } ]
    # j = json.dumps(data)
    # response = fibonacci_rpc.call(j)
    # data = json.loads(response)
    
    # print(" Got " + response)
    dispatcher = Dispatcher();
    if dispatcher.make(2):
        test_step(dispatcher)