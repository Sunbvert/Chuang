import pprint

class subprocess_test:
    def __init__(self):
        self.num = 1

    def start(self, q):
        # q.put(self.num)
        q.put("hello")
        q.put('hello again')