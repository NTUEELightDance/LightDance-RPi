import json
import os
import sys

import cmd2
from cmd2 import Fg, argparse, style

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from method import *
from pysocket import ZMQSocket

INTRO = r"""
    _   __ ______ __  __ ______ ______                         
   / | / //_  __// / / // ____// ____/                         
  /  |/ /  / /  / / / // __/  / __/                            
 / /|  /  / /  / /_/ // /___ / /___                            
/_/ |_/  /_/   \____//_____//_____/                            
    __     _         __     __   ____                          
   / /    (_)____ _ / /_   / /_ / __ \ ____ _ ____   _____ ___ 
  / /    / // __ `// __ \ / __// / / // __ `// __ \ / ___// _ \
 / /___ / // /_/ // / / // /_ / /_/ // /_/ // / / // /__ /  __/
/_____//_/ \__, //_/ /_/ \__//_____/ \__,_//_/ /_/ \___/ \___/ 
          /____/                                                                                   
"""


def dir_path(path):
    if os.path.isdir(path):
        file_list = ["LED.json", "OF.json"]
        for file in file_list:
            if not os.path.isfile(os.path.join(path, file)):
                raise argparse.ArgumentTypeError(f"{file} not found!")
        return path
    else:
        raise argparse.ArgumentTypeError("Directory not found!")


class LightDanceCLI(cmd2.Cmd):
    """LightDanceCLI"""

    def __init__(self):
        # CMD2 init
        self.intro = style(INTRO, fg=Fg.BLUE, bold=True)
        self.prompt = "LightDance CLI> "

        shortcuts = dict(cmd2.DEFAULT_SHORTCUTS)

        super().__init__(
            shortcuts=shortcuts, startup_script="./cli/startup", allow_cli_args=False
        )

        # ZMQ methods init
        self.socket = ZMQSocket(port=8000)
        self.METHODS = {
            "shutDown": ShutDown(),
            "reboot": Reboot(),
            "boardinfo": BoardInfo(),
            "uploadControl": UploadJsonFile(socket=self.socket),
            "load": Load(socket=self.socket),
            "play": Play(socket=self.socket),
            "pause": Pause(socket=self.socket),
            "stop": Stop(socket=self.socket),
            "statuslight": StatusLight(socket=self.socket),
            "eltest": ELTest(socket=self.socket),
            "oftest": OFTest(socket=self.socket),
            "lightall": LightAll(socket=self.socket),
            "darkall": DarkAll(socket=self.socket),
            "traversal": Traversal(socket=self.socket),
            "ledtest": LEDTest(socket=self.socket),
            "list": List(socket=self.socket),
            "quit": Quit(socket=self.socket),
            "send": Send(socket=self.socket),
            "red": Red(socket=self.socket),
            "green": Green(socket=self.socket),
            "blue": Blue(socket=self.socket),
            "stmInit": StmInit(socket=self.socket),
        }

        # vars init
        self.load = False
        self.control_path = "./data"
        self.dancer = os.environ["DANCER_NAME"]
        self.partMap = {}

    def response_parser(self, response: str):
        lines = response.split("\n")
        status = lines[0]

        content = "\n".join(lines[1:]).strip("\n")
        if content == "":
            content = "Success"

        if "error" in status.lower():
            self.perror(content)
        elif "success" in status.lower():
            self.poutput(content)
        elif "warning" in status.lower():
            self.pwarning(content)

    def do_boardinfo(self, args):
        """boardinfo"""
        info = self.METHODS["boardinfo"]()
        self.poutput(info)

    def do_reboot(self, args):
        """reboot"""
        self.METHODS["reboot"]()

    def do_shutdown(self, args):
        """shutdown"""
        self.METHODS["shutdown"]()

    def do_red(self, args):
        """red"""
        self.METHODS["red"]()

    def do_green(self, args):
        """green"""
        self.METHODS["green"]()

    def do_blue(self, args):
        """blue"""
        self.METHODS["blue"]()
    
    def do_stmInit(self, args):
        """stmInit"""
        self.METHODS["stmInit"]()

    # load [path]
    load_parser = cmd2.Cmd2ArgumentParser()
    load_parser.add_argument(
        "controlPath",
        nargs="?",
        default="data/",
        type=dir_path,
        help="Path to control directory.",
    )

    @cmd2.with_argparser(load_parser)
    def do_load(self, args):
        """Load control JSON file"""

        self.controlPath = args.controlPath

        file_list = ["LED.json", "OF.json"]
        for file in file_list:
            with open(os.path.join(self.controlPath, file), "r") as f:
                control = f.read()

            if not control:
                self.pwarning(f"Warning: {file} is empty")

        with open(
            os.path.join(self.controlPath, "dancers", f"{self.dancer}.json")
        ) as f:
            self.partMap = json.load(f)

        payload = {"path": self.controlPath}
        response = self.METHODS["load"](payload)

        self.response_parser(response)
        self.load = True

    complete_load = cmd2.Cmd.path_complete

    # play [start_time] [delay_time]
    play_parser = cmd2.Cmd2ArgumentParser()
    play_parser.add_argument(
        "start_time", nargs="?", default=0, type=int, help="start time"
    )
    play_parser.add_argument(
        "delay_time", nargs="?", default=0, type=int, help="delay time"
    )

    @cmd2.with_argparser(play_parser)
    def do_play(self, args):
        """play"""

        start_time = args.start_time
        delay_time = args.delay_time

        if not self.load:
            self.perror("Error: play failed, need to load first")
            return

        payload = {"start_time": str(start_time), "delay_time": str(delay_time)}
        response = self.METHODS["play"](payload)

        self.response_parser(response)

    def do_pause(self, args):
        """pause"""
        response = self.METHODS["pause"]()
        self.response_parser(response)

    def do_stop(self, args):
        """stop"""
        response = self.METHODS["stop"]()
        self.response_parser(response)

    def do_statuslight(self, args):  # TODO
        """statuslight"""
        response = self.METHODS["statuslight"]()
        self.response_parser(response)

    def do_list(self, args):  # TODO
        """list"""
        response = self.METHODS["list"]()
        self.response_parser(response)

    def do_quit(self, args):
        """quit"""
        # response = self.METHODS["quit"]()
        # self.response_parser(response)
        return 1

    send_parser = cmd2.Cmd2ArgumentParser()
    send_parser.add_argument(
        "message", nargs="?", default="Hello", type=str, help="message"
    )

    @cmd2.with_argparser(send_parser)
    def do_send(self, args):
        """send"""
        payload = {"message": args.message}
        response = self.METHODS["send"](payload)
        self.response_parser(response)

    # eltest [id] [brightness]
    eltest_parser = cmd2.Cmd2ArgumentParser()
    eltest_parser.add_argument("id", nargs="?", default=-1, type=int, help="id 0~31")
    eltest_parser.add_argument(
        "brightness", nargs="?", default=4095, type=int, help="brightness 0~4095"
    )

    @cmd2.with_argparser(eltest_parser)
    def do_eltest(self, args):
        """test el"""
        self.pwarning("Warning: DEPRECATED IN 2022")

        id = args.id
        brightness = args.brightness

        if brightness > 4095:
            self.pwarning(
                "Warning: brightness is bigger than 4095, light brightness as 4095"
            )
            brightness = 4095

        payload = {"id": str(id), "brightness": str(brightness)}
        response = self.METHODS["eltest"](payload)

        self.response_parser(response)

    # oftest [id] [color] [alpha]
    oftest_parser = cmd2.Cmd2ArgumentParser()
    oftest_parser.add_argument("channel", type=str, help="channel")
    oftest_parser.add_argument("color", nargs="+", type=str, help="RGB in HEX or R G B")
    oftest_parser.add_argument("alpha", type=int, help="brightness 0~15")

    @cmd2.with_argparser(oftest_parser)
    def do_oftest(self, args):
        """Test OF"""

        channel = args.channel
        color = args.color
        alpha = args.alpha

        try:
            channel = int(channel)
        except:
            channel = self.partMap["OFPARTS"][channel]

        if alpha > 15:
            self.pwarning("Warning: alpha is bigger than 15, light alpha as 15")
            alpha = 15

        if len(color) == 1:
            if color[0].startswith("0x"):
                color = int(color[0], 0)
            else:
                color = int(color[0], 16)
        else:
            color = (int(color[0]) << 16) + (int(color[1]) << 8) + int(color[2])

        payload = {"channel": str(channel), "color": str(color), "alpha": str(alpha)}
        response = self.METHODS["oftest"](payload)

        self.response_parser(response)

    # lightall [id] [color] [alpha]
    lightall_parser = cmd2.Cmd2ArgumentParser()
    lightall_parser.add_argument(
        "color", nargs="+", type=str, help="RGB in HEX or R G B"
    )
    lightall_parser.add_argument("alpha", type=int, help="brightness 0~15")

    @cmd2.with_argparser(lightall_parser)
    def do_lightall(self, args):
        """light all"""

        color = args.color
        alpha = args.alpha

        if alpha > 15:
            self.pwarning("Warning: alpha is bigger than 15, light alpha as 15")
            alpha = 15

        if len(color) == 1:
            if color[0].startswith("0x"):
                color = int(color[0], 0)
            else:
                color = int(color[0], 16)
        else:
            color = (int(color[0]) << 16) + (int(color[1]) << 8) + int(color[2])

        payload = {"color": str(color), "alpha": str(alpha)}
        response = self.METHODS["lightall"](payload)

        self.response_parser(response)

    def do_darkall(self, args):
        """darkall"""
        response = self.METHODS["darkall"]()
        self.response_parser(response)

    # ledtest [id] [color] [alpha]
    ledtest_parser = cmd2.Cmd2ArgumentParser()
    ledtest_parser.add_argument("channel", type=str, help="channel")
    ledtest_parser.add_argument(
        "color", nargs="+", type=str, help="RGB in HEX or R G B"
    )
    ledtest_parser.add_argument("alpha", type=int, help="brightness 0~15")

    @cmd2.with_argparser(oftest_parser)
    def do_ledtest(self, args):
        """Test LED"""

        channel = args.channel
        color = args.color
        alpha = args.alpha

        try:
            channel = int(channel)
        except:
            channel = self.partMap["LEDPARTS"][channel]

        if alpha > 15:
            self.pwarning("Warning: alpha is bigger than 15, light alpha as 15")
            alpha = 15

        if len(color) == 1:
            if color[0].startswith("0x"):
                color = int(color[0], 0)
            else:
                color = int(color[0], 16)
        else:
            color = (int(color[0]) << 16) + (int(color[1]) << 8) + int(color[2])

        payload = {"channel": str(channel), "color": str(color), "alpha": str(alpha)}
        response = self.METHODS["ledtest"](payload)

        self.response_parser(response)

    # sendlight [id] [vector]
    sendlight_parser = cmd2.Cmd2ArgumentParser()
    sendlight_parser.add_argument("id", nargs="?", default=-1, type=int, help="id 0~31")
    sendlight_parser.add_argument(
        "vector", nargs="?", default=4095, type=int, help="brightness 0~4095"
    )

    @cmd2.with_argparser(sendlight_parser)
    def do_sendlight(self, args):
        """send light"""

        id = args.id
        vector = args.vector

        payload = {"id": str(id), "vector": str(vector)}
        response = self.METHODS["sendlight"](payload)

        self.response_parser(response)

    # setofchannel [partName] [channel]
    setofchannel_parser = cmd2.Cmd2ArgumentParser()
    setofchannel_parser.add_argument("partName", type=str, help="partName")
    setofchannel_parser.add_argument("channel", type=int, help="channel")

    @cmd2.with_argparser(setofchannel_parser)
    def do_setofchannel(self, args):
        """Set OF Channel"""

        partName = args.partName
        channel = args.channel

        self.partMap["OFPARTS"][partName] = channel

        with open(
            os.path.join(self.controlPath, "dancers", f"{self.dancer}.json"), "w"
        ) as f:
            json.dump(self.partMap, f)

        payload = {"path": self.controlPath}
        response = self.METHODS["load"](payload)

        self.response_parser(response)
        self.load = True

    # setledchannel [partName] [channel]
    setledchannel_parser = cmd2.Cmd2ArgumentParser()
    setledchannel_parser.add_argument("partName", type=str, help="partName")
    setledchannel_parser.add_argument("channel", type=int, help="channel")

    @cmd2.with_argparser(setledchannel_parser)
    def do_setledchannel(self, args):
        """Set LED Channel"""

        partName = args.partName
        channel = args.channel

        self.partMap["LEDPARTS"][partName]["id"] = channel

        with open(
            os.path.join(self.controlPath, "dancers", f"{self.dancer}.json"), "w"
        ) as f:
            json.dump(self.partMap, f)

        payload = {"path": self.controlPath}
        response = self.METHODS["load"](payload)

        self.response_parser(response)
        self.load = True

    def do_traversal(self, args):
        """traversal"""
        response = self.METHODS["traversal"]()
        self.response_parser(response)


if __name__ == "__main__":
    app = LightDanceCLI()
    app.debug = True
    sys.exit(app.cmdloop())
