import zmq
xpub_addr = 'tcp://127.0.0.1:5555'
context = zmq.Context()
subscriberSocket = context.socket(zmq.SUB)
subscriberSocket.connect(xpub_addr)
subscriberSocket.setsockopt(zmq.SUBSCRIBE, b"xxx")
while True:
    if subscriberSocket.poll(timeout=1000):
        message = subscriberSocket.recv_multipart()
        print(message)
