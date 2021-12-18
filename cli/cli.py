import sys
import os
import cmd2
from cmd2 import Bg, Fg, style, argparse

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
        self.load = False

        self.intro = style(INTRO, fg=Fg.BLUE, bold=True)
        self.prompt = "LightDance CLI> "

        shortcuts = dict(cmd2.DEFAULT_SHORTCUTS)

        super().__init__(shortcuts=shortcuts)

    # load [path]
    load_parser = cmd2.Cmd2ArgumentParser()
    load_parser.add_argument(
        "control_path",
        nargs="?",
        default="control.json",
        type=file_path,
        help="Path to control JSON file.",
    )

    @cmd2.with_argparser(load_parser)
    def do_load(self, args):
        """Load control JSON file"""

        with open(args.control_path, "r") as f:
            control_path = f.read()

        if not control_path:
            self.pwarning("Warning: control.json is empty")

        self.poutput(f"load: {args.control_path}")
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
        if not self.load:
            self.perror("Error: play failed, need to load first")
            return

        self.poutput(f"played {args.start_time} {args.delay_time}")

    def do_stop(self, args):
        """stop"""
        self.poutput("stop")

    def do_statuslight(self, args):
        """statuslight"""
        self.poutput("statuslight")

    def do_list(self, args):
        """list"""
        self.poutput("list")

    # eltest [id] [brightness]
    eltest_parser = cmd2.Cmd2ArgumentParser()
    eltest_parser.add_argument("id", nargs="?", default=-1, type=int, help="id 0~31")
    eltest_parser.add_argument(
        "brightness", nargs="?", default=4095, type=int, help="brightness 0~4095"
    )

    @cmd2.with_argparser(eltest_parser)
    def do_eltest(self, args):
        """test el"""
        if args.brightness > 4095:
            self.pwarning(
                "Warning: brightness is bigger than 4095, light brightness as 4095"
            )
            args.brightness = 4095

        self.poutput(f"eltest {args.id} {args.brightness}")

    def do_ledtest(self, args):
        """test led"""
        self.poutput("ledtest")


if __name__ == "__main__":
    app = LightDanceCLI()
    sys.exit(app.cmdloop())
