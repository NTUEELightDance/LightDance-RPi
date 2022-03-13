from .baseMethod import BaseMethod


# Play
class Play(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(
            ["play", "%d %d" % (payload["start_time"], payload["delay_time"])]
        )
        return response
