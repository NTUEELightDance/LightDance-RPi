import json

# peoples = {
#     "rayh": [
#         {"time": 0, "fade": False},
#         {"time": 1, "fade": False},
#         {"time": 2, "fade": False},
#     ]
# }

with open("light_time.json", "r") as f:
    peoples = json.load(f)

with open("control_umbrella.json", "r") as f:
    control = json.load(f)

for name, v in peoples.items():

    frames = v

    # OF
    OF = []
    for frame in frames:
        f = {}
        f["start"] = frame["time"]
        f["fade"] = frame["fade"]
        f["status"] = {}

        for i, c in enumerate(control["OFPARTS"]):
            f["status"][c] = [0x01, 0x00, 0xC7, frame["alpha"]]

        OF.append(f)

    with open(f"umb/{name}_OF.json", "w") as f:
        json.dump(OF, f, indent=2)

    # LED
    LED = {}
    for k, v in control["LEDPARTS"].items():
        LED[k] = []
        for frame in frames:
            f = {}
            f["start"] = frame["time"]
            f["fade"] = frame["fade"]
            f["status"] = [[0x00, 0x5E, 0xFF, frame["alpha"]] * v["len"]]
            LED[k].append(f)

    with open(f"umb/{name}_LED.json", "w") as f:
        json.dump(LED, f, indent=2)
