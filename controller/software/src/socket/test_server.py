import json
import socket

def send_json(conn, json_data):
    json_str = json.dumps(json_data)
    json_str = json_str.encode()
    conn.sendall(json_str)

def main():
    # Create a socket object
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Bind the socket to a specific address and port
    server_socket.bind(('127.0.0.1', 9002))

    # Listen for incoming connections
    server_socket.listen(5)

    while True:
        # Accept a connection
        conn, addr = server_socket.accept()
        print(f'Connected by {addr}')

        # Read a json file
        with open('data.json', 'r') as json_file:
            json_data = json.load(json_file)

        # Send the json file to the client
        send_json(conn, json_data)

        # Close the connection
        conn.close()

if __name__ == '__main__':
    main()