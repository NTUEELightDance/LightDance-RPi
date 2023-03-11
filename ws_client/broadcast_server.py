from websocket_server import WebsocketServer
import json
from config import *

# Called for every client connecting (after handshake)
def new_client(client, server):
	print("New client connected and was given id %d" % client['id'])
	# server.send_message_to_all("Hey all, a new client has joined us")
	
	more_client = input("Is there any more client?(y/n): ")
	command_json = dict()
	mode = ""

	while more_client == 'n' or more_client == "N":

		if mode == "command":
			command = input("Enter command: ")
			if command == "exit":
				mode = ""
			else:
				command_json["action"] = "command"
				command_json["payload"] = command.split()
				print(command_json)
				server.send_message_to_all(json.dumps(command_json))

		elif mode == "upload":
			command_json["action"] = "upload"
			control_json = json.loads(open("../data/control.json", "r").read())
			of_json = json.loads(open("../data/OF.json", "r").read())
			led_json = json.loads(open("../data/LED.json", "r").read())
			command_json["payload"] = [control_json, of_json, led_json]
			print(command_json)
			server.send_message_to_all(json.dumps(command_json))
			mode = ""

		elif mode == "sync":
			if command == "exit":
				mode = ""
			else:
				command_json["action"] = "sync"
				command_json["payload"] = []
				print(command_json)
				server.send_message_to_all(json.dumps(command_json))

		elif mode == "exit":
			break
		else:
			mode = input("Enter mode: ")



# Called for every client disconnecting
def client_left(client, server):
	print("Client(%d) disconnected" % client['id'])


# Called when a client sends a message
def message_received(client, server, message):
	if len(message) > 200:
		message = message[:200]+'..'
	print("Client(%d) said: %s" % (client['id'], message))

server = WebsocketServer(host = SERVER_IP, port = SERVER_PORT)
server.set_fn_new_client(new_client)
server.set_fn_client_left(client_left)
server.set_fn_message_received(message_received)
server.run_forever()



