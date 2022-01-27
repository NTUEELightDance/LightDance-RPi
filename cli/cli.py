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


def file_path(path):
    if os.path.isfile(path):
        return path
    else:
        raise argparse.ArgumentTypeError("File not found!")


class LightDanceCLI(cmd2.Cmd):
    """LightDanceCLI"""

    def __init__(self):
        # CMD2 init
        self.intro = style(INTRO, fg=Fg.BLUE, bold=True)
        self.prompt = "LightDance CLI> "

        shortcuts = dict(cmd2.DEFAULT_SHORTCUTS)

        super().__init__(shortcuts=shortcuts, startup_script="./cli/startup")

        # ZMQ methods init
        self.socket = ZMQSocket(port=8000)
        self.METHODS = {
            "shutDown": ShutDown(),
            "reboot": Reboot(),
            "boardinfo": BoardInfo(),
            "uploadControl": UploadJsonFile(socket=self.socket),
            "load": Load(socket=self.socket),
            "play": Play(socket=self.socket),
            "stop": Stop(socket=self.socket),
            "statuslight": StatusLight(socket=self.socket),
            "eltest": ELTest(socket=self.socket),
            "ledtest": LEDTest(socket=self.socket),
            "list": List(socket=self.socket),
            "quit": Quit(socket=self.socket),
            "send": Send(socket=self.socket),
        }

        # vars init
        self.load = False

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

    # load [path]
    load_parser = cmd2.Cmd2ArgumentParser()
    load_parser.add_argument(
        "control_path",
        nargs="?",
        default="data/control.json",
        type=file_path,
        help="Path to control JSON file.",
    )

    @cmd2.with_argparser(load_parser)
    def do_load(self, args):
        """Load control JSON file"""

        control_path = args.control_path
        with open(control_path, "r") as f:
            control = f.read()

        if not control:
            self.pwarning("Warning: control.json is empty")

        payload = {"path": control_path}
        response = self.METHODS["load"](payload)

        self.poutput(response)
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

        self.poutput(response)

    def do_stop(self, args):
        """stop"""
        response = self.METHODS["stop"]()
        self.poutput(response)

    def do_statuslight(self, args):  # TODO
        """statuslight"""
        response = self.METHODS["statuslight"]()
        self.poutput(response)

    def do_list(self, args):  # TODO
        """list"""
        response = self.METHODS["list"]()
        self.poutput(response)

    def do_quit(self, args):
        """quit"""
        response = self.METHODS["quit"]()
        self.poutput(response)
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
        self.poutput(response)

    # eltest [id] [brightness]
    eltest_parser = cmd2.Cmd2ArgumentParser()
    eltest_parser.add_argument("id", nargs="?", default=-1, type=int, help="id 0~31")
    eltest_parser.add_argument(
        "brightness", nargs="?", default=4095, type=int, help="brightness 0~4095"
    )

    @cmd2.with_argparser(eltest_parser)
    def do_eltest(self, args):
        """test el"""

        id = args.id
        brightness = args.brightness

        if brightness > 4095:
            self.pwarning(
                "Warning: brightness is bigger than 4095, light brightness as 4095"
            )
            brightness = 4095

        payload = {"id": str(id), "brightness": str(brightness)}
        response = self.METHODS["eltest"](payload)

        self.poutput(response)

    def do_ledtest(self, args):  # TODO
        """test led"""
        response = self.METHODS["ledtest"]()
        self.poutput(response)


if __name__ == "__main__":
    app = LightDanceCLI()
    app.debug = True
    sys.exit(app.cmdloop())
