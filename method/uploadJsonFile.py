import json
import os

from .baseMethod import BaseMethod

# Load timeline.json to local storage
# Payload must be a map, with two keys (mentioned below)
# dir: directory to save the timeline file
# file: the file itself


class UploadJsonFile(BaseMethod):
    def method(self, payload):
        save_dir = payload["dir"]
        data = payload["file"]

        if not os.path.exists(save_dir):
            os.makedirs(save_dir)

        file_save_path = os.path.join(save_dir, "control.json")
        with open(file_save_path, "w") as json_file:
            json.dump(obj=data, fp=json_file, indent=4)

        print("File saved successfully at %s" % file_save_path)
