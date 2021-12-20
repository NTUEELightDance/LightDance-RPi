import baseMethod
import os

class BoardInfo(baseMethod):
    def method(self, payload):
        info = [
            "boardInfo",
            {
                "name": os.name,
                "type": "dancer",
                "OK": True,
                "msg": "Success"
            }
        ]
        return info