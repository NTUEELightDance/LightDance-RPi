from .baseMethod import BaseMethod

# StmInit
class StmInit(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("stmInit")
        return response