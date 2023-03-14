import json
import random

# control.json
control = {"fps": 30, "OFPARTS": {}, "LEDPARTS": {}}

for i in range(35):
    control["OFPARTS"][f"OF{i}"] = i

for i in range(8):
    control["LEDPARTS"][f"LED{i}"] = {"id": i, "len": 100}

with open("control.json", "w") as f:
    json.dump(control, f, indent=2)
    print("control.json generated")

# OF.json
OF = []
frames = [
    [30, 0, 0, 10],
    [0, 0, 0, 0],
    [0, 30, 0, 10],
    [0, 0, 0, 0],
    [0, 0, 30, 10],
    [0, 0, 0, 0],
    [30, 30, 0, 10],
    [0, 0, 0, 0],
    [30, 0, 30, 10],
    [0, 0, 0, 0],
    [0, 30, 30, 10],
    [0, 0, 0, 0],
    [30, 30, 30, 10],
]

for i, frame in enumerate(frames):
    status = {}
    for j in range(35):
        status[f"OF{j}"] = frame
    OF.append({"start": i * 5000, "fade": True, "status": status})

with open("OF.json", "w") as f:
    json.dump(OF, f, indent=2)
    print("OF.json generated")

# LED.json
LED = {}
for i in range(8):
    LED[f"LED{i}"] = []
    for j, frame in enumerate(frames):
        status = []
        for _ in range(100):
            status.append(frame)
        LED[f"LED{i}"].append({"start": j * 5000, "fade": True, "status": status})

with open("LED.json", "w") as f:
    json.dump(LED, f, indent=2)
    print("LED.json generated")
