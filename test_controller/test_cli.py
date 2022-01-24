import zmq

context = zmq.Context()
print("Connecting to server...")
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:8000")

while True:
    msg = input("CLI msg：")
    socket.send_string(msg)

    reply = socket.recv_multipart()
    print("Received reply: ", reply.decode())
