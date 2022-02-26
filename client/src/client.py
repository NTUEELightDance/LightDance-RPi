#from asyncio.windows_events import NULL
import websocket
import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from method import *
from pysocket import ZMQSocket

boardname = "lightdancer-1" # use"lightdancer-1" to test
cmdlist = ["shutDown","reboot","boardinfo","uploadControl","load","play","pause","stop","statuslight","eltest","ledtest","list","quit","send"] 

class Client:
    def __init__(self):
        super(Client,self).__init__()
        self.url = "ws://localhost:8080"
        self.cmd = ""
        self.paylaod = {}
        
        ######zmq#######
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
            "ledtest": LEDTest(socket=self.socket),
            "list": List(socket=self.socket),
            "quit": Quit(socket=self.socket),
            "send": Send(socket=self.socket),
        }
        #########
    
    def on_message(self, ws, message):
        cmd , payload = self.ParseServerData(message)
        if (self.Check(ws,cmd,payload) != False):
            ws.send(json.dumps([
                cmd,
                {
                    "type": "dancer",
                    "name": boardname,
                    "OK": True,
                    "msg": cmd+" success",
                },
            ]))
            if (payload == None):
                self.METHODS[cmd]()
            else:
                self.METHODS[cmd](payload)
            print("Send message to rpi complete")
        else:
            print("Failed")
        
    def on_open(self,ws):
        print("Successfully on_open") #Print Whether successfully on_open
        ws.send(json.dumps([
                "boardInfo",
                {
                    "type": "dancer",
                    "name": boardname,
                    "OK": True,
                    "msg": "Connect Success",
                },
            ]))
    
    def on_close(self,ws):
        print("close")
    
    ####### on_error ########
    # def on_error(self,ws,error):
    #     print("The error is %s" %error)
    
    def startclient(self):
        while True:
            try:
                ws = websocket.WebSocketApp(self.url,
                                                on_message = self.on_message,
                                                on_open = self.on_open, 
                                                on_close = self.on_close)
                                                # on_error = self.on_error)
                ws.run_forever()
            except websocket.WebSocketException:
                print("Fail to connect")
                
   
    
    def ParseServerData(self, message):
        #print(message)
        try:
            #print(type(message))
            self.dictclient = json.loads(message)
            if (len(self.dictclient)!=1):
                return self.dictclient['cmd'] , self.dictclient['payload']
            else:
                return self.dictclient['cmd'] , None
        except:
            print("Faild at json")
    
    def Check(self,ws,cmd,payload):
        for i in cmdlist :
            if (i==cmd):
                return True
        print("Check Failed")
        ws.send(json.dumps([
                cmd,
                {
                    "type": "dancer",
                    "name": boardname,
                    "OK": False,
                    "msg": "This Command doesn't exist",
                },
            ]))  
        return False 
                                              
Test = Client()
Test.startclient()
   