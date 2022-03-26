import json
import os

from .baseMethod import BaseMethod
from .ledtest import LEDTest
from .oftest import OFTest


# Statuslight
class StatusLight(BaseMethod):
    def method(self, payload=None):
        # response = self.socket.send("statuslight")
        statusList = json.loads(payload)

        dancer = os.environ["DANCER_NAME"]

        with open(os.path.join("./data", "dancers", f"{dancer}.json")) as f:
            partMap = json.load(f)

        for status in statusList["OFPARTS"].items():
            payload = {
                "channel": str(partMap["OFPARTS"][status[0]]),
                "color": str(status[1]["colorCode"]),
                "alpha": str(status[1]["alpha"]),
            }
            OFTest(self.socket)(payload)

        for status in statusList["LEDPARTS"].items():
            payload = {
                "channel": str(partMap["LEDPARTS"][status[0]]["id"]),
                "color": str(status[1][0]["colorCode"]),
                "alpha": str(status[1][0]["alpha"]),
            }
            LEDTest(self.socket)(payload)


"""
{
    "OFPARTS":{
            "Calf_R": { "colorCode": 16711680, "alpha": 10 },
            "Thigh_R": { "colorCode": 16711680, "alpha": 10 },
            "LymphaticDuct_R": { "colorCode": 16711680, "alpha": 10 },
            "Waist_R": { "colorCode": 16711680, "alpha": 10 },
            "Arm_R": { "colorCode": 16711680, "alpha": 10 },
            "Shoulder_R": { "colorCode": 16711680, "alpha": 10 },
            "CollarBone_R": { "colorCode": 16711680, "alpha": 10 },
            "Chest": { "colorCode": 16711680, "alpha": 10 },
            "Visor": { "colorCode": 16711680, "alpha": 10 },
            "Ear_R": { "colorCode": 16711680, "alpha": 10 },
            "Calf_L": { "colorCode": 16711680, "alpha": 10 },
            "Thigh_L": { "colorCode": 16711680, "alpha": 10 },
            "LymphaticDuct_L": { "colorCode": 16711680, "alpha": 10 },
            "Waist_L": { "colorCode": 16711680, "alpha": 10 },
            "CollarBone_L": { "colorCode": 16711680, "alpha": 10 },
            "Arm_L": { "colorCode": 16711680, "alpha": 10 },
            "Ear_L": { "colorCode": 16711680, "alpha": 10 },
            "Shoulder_L": { "colorCode": 16711680, "alpha": 10 },
            "Glove_L": { "colorCode": 16711680, "alpha": 10 },
            "Glove_R": { "colorCode": 16711680, "alpha": 10 }
        },
        "LEDPARTS":{
            Glove_L_LED:[
                { "colorCode": 16777005, "alpha": 10 },
                { "colorCode": 16777005, "alpha": 10 }
            ],
            Glove_R_LED:[
                { "colorCode": 16777005, "alpha": 10 },
                { "colorCode": 16777005, "alpha": 10 }
            ]
        }
}
"""
