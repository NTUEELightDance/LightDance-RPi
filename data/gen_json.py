import json
brightness = 150
frames = [
    # [r, g, b, a]
    [255, 0, 0, brightness],  # red
    [255, 255, 0, brightness],  # yellow
    [0, 255, 0, brightness],  # green
    [0, 255, 255, brightness],  # cyan
    [0, 0, 255, brightness],  # blue
    [255, 0, 255, brightness],  # purple
    [255, 0, 0, brightness],  # red
]
TIME = 1000
FADE = True

LED_LEN = 100

# control.json
control = {"fps": 30, "OFPARTS": {}, "LEDPARTS": {}}

for i in range(35):
    control["OFPARTS"][f"OF{i}"] = i

for i in range(8):
    control["LEDPARTS"][f"LED{i}"] = {"id": i, "len": LED_LEN}

with open("control.json", "w") as f:
    json.dump(control, f, indent=2)
    print("control.json generated")

# OF.json
OF = []

for i, frame in enumerate(frames):
    status = {}
    for j in range(35):
        status[f"OF{j}"] = frame
    OF.append({"start": i * TIME, "fade": FADE, "status": status})

with open("OF.json", "w") as f:
    json.dump(OF, f, indent=2)
    print("OF.json generated")

# LED.json
LED = {}
for i in range(8):
    LED[f"LED{i}"] = []
    for j, frame in enumerate(frames):
        status = []
        for _ in range(LED_LEN):
            status.append(frame)
        LED[f"LED{i}"].append({"start": j * TIME, "fade": FADE, "status": status})

with open("LED.json", "w") as f:
    json.dump(LED, f, indent=2)
    print("LED.json generated")
