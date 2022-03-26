import json
import os
import sys

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from method import *
from pysocket import ZMQSocket

mysocket = ZMQSocket(port=8000)

status = """
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
            "Glove_L_LED":[
                { "colorCode": 16777005, "alpha": 10 },
                { "colorCode": 16777005, "alpha": 10 }
            ],
            "Glove_R_LED":[
                { "colorCode": 16777005, "alpha": 10 },
                { "colorCode": 16777005, "alpha": 10 }
            ]
        }
}
"""

StatusLight(socket=mysocket)(status)
