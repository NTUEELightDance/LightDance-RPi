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

        # if not os.path.exists(save_dir.split('/')[0]):
        #     os.makedirs(save_dir)

        file_save_path = save_dir
        with open(file_save_path, "w") as json_file:
            json.dump(obj=data, fp=json_file, indent=4)

        print("File saved successfully at %s" % file_save_path)
        if "LED" in file_save_path:
            return f"[ uploadLed / Success ] File saved at {file_save_path}"
        elif "OF" in file_save_path:
            return f"[ uploadOf / Success ] File saved at {file_save_path}"
