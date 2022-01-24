from .baseMethod import BaseMethod


# Send
class Send(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(payload["message"])
        return response
