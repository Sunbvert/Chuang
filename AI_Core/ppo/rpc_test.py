from lib.rabbitmq import RpcClient
import json

if __name__ == "__main__":
    fibonacci_rpc = RpcClient()

    print(" [x] Requesting fib(30)")
    data = [ { 'a' : 1, 'b' : 2, 'c' : 3, 'd' : 4, 'e' : 5 } ]
    j = json.dumps(data)
    response = fibonacci_rpc.call(j)
    data = json.loads(response)
    
    print(" Got " + response)