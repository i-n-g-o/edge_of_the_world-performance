import zmq
xsub_addr = 'tcp://127.0.0.1:5556'
context = zmq.Context()
publisherSocket = context.socket(zmq.PUB)
publisherSocket.connect(xsub_addr)
publishName = b"xxx"; #.encode('utf-8')

while True:
    message = input('input the message:')
    publisherSocket.send_multipart([publishName,message.encode('utf-8')])
    print(message)
