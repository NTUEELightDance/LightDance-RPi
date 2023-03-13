import json
import os

base_path = "/home/pi/LightDance-RPi"

if not os.path.exists(os.path.join(base_path, "data/color_palette.json")):
    if not os.path.exists(os.path.join(base_path, "data/control.json")):
        print("control.json not found")

    with open(os.path.join(base_path, "data/control.json"), "r") as f:
        control = json.load(f)

    template = {}
    for k, v in control["OFPARTS"].items():
        template[k] = [0, 0, 0, 0]

    for k, v in control["LEDPARTS"].items():
        # print(k, v)
        length = v["len"]
        template[k] = []
        for i in range(length):
            template[k].append({"id": i, "color": [0, 0, 0, 0]})

    with open(os.path.join(base_path, "data/color_palette.json"), "w") as f:
        json.dump(template, f, indent=4)

    print("color_palette.json created")
    exit(0)

else:
    os.system("color_palette")
