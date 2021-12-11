const WebSocket = require("ws");
const { spawn } = require("child_process");
const fs = require("fs");
const path = require("path");
const os = require("os");
const shell = require("shelljs");
const readline = require("readline");

const NtpClient = require('./ntp/ntpClient')

class RpiSocket {
    constructor() {
        this.wsClient = null;
        this.controller = null; //c++ child process
        this.cmdFromServer = null;
        this.connected = false;
        this.needToReconnect = true;
        this.musicPlaying = false;
        this.ntpClient = new NtpClient((time) => {
            console.log(`Set to time ${time}`)
        })
        this.init();
    }
    init = () => {
        this.connectWebsocket();
    };
    connectWebsocket = () => {
        this.wsClient = new WebSocket("ws://192.168.0.200:8080");
        if (this.wsClient.readyState !== WebSocket.CONNECTING) {
            setTimeout(() => {
                this.init();
            }, 3000);
            return;
        } else {
            this.listeningServer();
        }
    };
    listeningServer = () => {
        this.wsClient.onopen = () => {
            this.connectedBefore = true;
            console.log("Websocket connected.");
            this.sendDataToServer([
                "boardInfo",
                {
                    //send boardInfo while connected to server
                    type: "dancer",
                    name: os.hostname(),
                    OK: true,
                    msg: "Connect Success",
                },
            ]);
        };
        this.wsClient.onerror = (err) => {
            console.log(`Websocket error: ${err.message}`);
        };
        this.wsClient.onmessage = (mes) => {
            const data = mes.data;
            console.log(`----------------------------------------`);
            console.log(`Data from server: ${data}`);
            this.parseServerData(data);
        };
        this.wsClient.onclose = (e) => {
            console.log("Websocket client closed.");
            if (!this.musicPlaying && this.controller) this.controller.kill(); //若音樂在播而不小心斷線，就不管

            if (this.needToReconnect) {
                console.log("Websocket client reconnecting to server...");
                setTimeout(() => {
                    this.init();
                }, 3000);
                return;
            } else process.exit();
        };
    };
    listeningCpp = () => {
        const parseCppData = this.parseCppData;
        // Listening to stdout
        const rl = readline.createInterface({
            input: this.controller.stdout,
        });

        rl.on("line", function (line) {
            const data = line.trim();
            if (data.length) {
                console.log(`Data from C++: ${data}`);
                parseCppData(data);
            }
        });

        // Listening to error
        const rlErr = readline.createInterface({
            input: this.controller.stderr,
        });

        rlErr.on("line", function (line) {
            const data = line.trim();
            if (data.length) {
                console.log(`Data from C++ Error: ${data}`);
                parseCppData(data);
            }
        });
    };
    parseServerData = (mes) => {
        const [task, payload] = this.parseData(mes);
        console.log("Command: ", task, ", Payload: ", payload);
        this.cmdFromServer = task;
        // if (this.controller !== null) this.listeningCpp();
        try {
            switch (this.cmdFromServer) {
                case "start": {
                    //start c++ file
                    if (this.controller) {
                        console.log("Killing running C++.");
                        this.controller.kill("SIGKILL");
                        this.controller = null;
                        console.log("Running C++ is killed");
                    }
                    try {
                        this.controller = spawn(
                            path.join(__dirname, `../../controller/controller`),
                            [payload]
                        ); //use ./test to test, change to ./controller for real time deployment
                        this.listeningCpp();
                    } catch (err) {
                        console.error(err);
                        this.controller = null;
                    }
                    break;
                }
                case "play": {
                    //start playing
                    const { startTime, delay, sysTime } = payload; //從整首歌的第幾秒播放, delay 多久, 系統時間多少開始
                   // this.sendDataToCpp(`play ${startTime} ${delay} ${sysTime}`);
                    const nowTime = Date.now(); 
                    const de = sysTime - nowTime;
                    console.log(`nowTime ${nowTime}, sysTime ${sysTime} delay ${de}`);
                    setTimeout(() => {
                        shell.exec("/home/pi/playSong.sh");
                    }, delay + 300);
                    break;
                } //back to server的部分還未確定
                case "pause": {
                    //pause playing
                    if (this.controller) this.controller.kill("SIGINT");
                    break;
                }
                case "stop": {
                    //stop playing(prepare to start time)
                    this.sendDataToCpp("stop"); //another SIG
                    break;
                }
                case "load": {
                    //call cpp to load play file.json
                    this.sendDataToCpp("load");
                    break;
                }
                case "terminate": {
                    //terminate c++ file
                    // if (this.controller !== null) this.controller.kill("SIGKILL");
                    this.sendDataToCpp("quit");
                    this.controller = null;
                    this.sendDataToServer([
                        "terminate",
                        {
                            OK: true,
                            msg: "terminated success",
                        },
                    ]);
                    break;
                }
                case "lightCurrentStatus": {
                    if (!fs.existsSync(path.join(__dirname, "../../data")))
                        fs.mkdirSync(path.join(__dirname, "../../data"));
                    fs.writeFileSync(
                        path.join(__dirname, "../../data/status.json"),
                        JSON.stringify(payload),
                        (err) => {
                            if (err) {
                                console.log("lightCurrentStatus file failed.");
                                this.sendDataToServer([
                                    "lightCurrentStatus",
                                    {
                                        OK: false,
                                        msg: "upload failed",
                                    },
                                ]);
                                throw err;
                            } else {
                                console.log("lightCurrentStatus file success.");
                                this.sendDataToServer([
                                    "lightCurrentStatus",
                                    {
                                        OK: true,
                                        msg: "upload success",
                                    },
                                ]);
                            }
                        }
                    );
                    this.sendDataToCpp("statuslight");
                    break;
                }
                //above are commands sending to clientApp(controller.cpp)
                case "kick": {
                    //reconnect to websocket server
                    this.wsClient.close();
                    this.wsClient.onclose = (e) => {
                        console.log(
                            "Websocket client is manually diconnect,reconnecting to websocket server..."
                        );
                    };
                    delete this.wsClient;
                    this.connectWebsocket();

                    if (this.controller) this.controller.kill("SIGKILL");
                    this.controller = null;
                    this.cmdFromServer = null;
                    this.musicPlaying = false;
                    break;
                }
                case "uploadControl": {
                    //load timeline.json to local storage(./current)
                    //console.log(payload);
                    if (!fs.existsSync(path.join(__dirname, "../../data")))
                        fs.mkdirSync(path.join(__dirname, "../../data"));
                    fs.writeFile(
                        path.join(__dirname, "../../data/control.json"),
                        JSON.stringify(payload),
                        (err) => {
                            if (err) {
                                console.log("Upload control file failed.");
                                this.sendDataToServer([
                                    "uploadControl",
                                    {
                                        OK: false,
                                        msg: "upload failed",
                                    },
                                ]);
                                throw err;
                            } else {
                                console.log("Upload control file success.");
                                this.sendDataToServer([
                                    "uploadControl",
                                    {
                                        OK: true,
                                        msg: `upload success length=${payload.length}`,
                                    },
                                ]);
                            }
                        }
                    );
                    break;
                }
                case "uploadLED": {
                    //load led picture files to ./control/LED
                    if (!fs.existsSync(path.join(__dirname, "../../asset")))
                        fs.mkdir(
                            fs.existsSync(path.join(__dirname, "../../asset"))
                        );
                    fs.writeFile(
                        path.join(__dirname, "../../asset/LED.json"),
                        JSON.stringify(payload),
                        (err) => {
                            if (err) {
                                console.log("Upload LED file failed.");
                                this.sendDataToServer([
                                    "uploadLED",
                                    {
                                        OK: false,
                                        msg: "upload failed",
                                    },
                                ]);
                                throw err;
                            } else {
                                console.log("Upload LED file success.");
                                this.sendDataToServer([
                                    "uploadLED",
                                    {
                                        OK: true,
                                        msg: `upload success ${Object.keys(payload).length}`,
                                    },
                                ]);
                            }
                        }
                    );
                    break;
                }
                case "shutDown": {
                    //shut down Rpi computer
                    this.needToReconnect = false;
                    shell.exec("sudo halt");
                    break;
                }
                case "reboot": {
                    //reopen Rpi computer
                    shell.exec("reboot -h 0");
                    break;
                }
                case "sync": {
                    //payload 至少會有時間，將Rpi的時間與電腦同步\
                    this.ntpClient.startTimeSync((transmitDelay, offset) => {
                        this.sendDataToServer([
                            "sync",
                            {
                                OK: 1,
                                msg: `transmit delay ${transmitDelay}, offset ${offset}`
                            }
                        ])
                    });
                    break;
                }
                case "boardInfo": {
                    this.sendDataToServer([
                        "boardInfo",
                        {
                            //send boardInfo while connected to server
                            name: os.hostname(),
                            type: "dancer",
                            OK: true,
                            msg: "Success",
                        },
                    ]);
                    break;
                }
            }
        } catch (err) {
            if (this.controller) {
                this.sendDataToServer([
                    task,
                    {
                        OK: 0,
                        msg:
                            'controller isn\'t started yet or disconnect unexpected, please press "start" button again',
                    },
                ]);
            }
            // else if (){

            // }
            else if (err instanceof TypeError) {
            } else if (err instanceof SyntaxError) {
            } else if (err instanceof EvalError) {
            } else {
                this.sendDataToServer([
                    task,
                    {
                        OK: 0,
                        msg: err,
                    },
                ]);
            }
        }
    };
    parseCppData = (mes) => {
        const message = mes.toString().trim();
        const OK = message.toLowerCase().includes("success");

        this.sendDataToServer([
            this.cmdFromServer,
            {
                OK,
                msg: message,
            },
        ]);
    };
    sendDataToServer = (data) => {
        console.log("Data to Server: ", data);
        console.log("----------------------------------------");
        this.wsClient.send(JSON.stringify(data));
    };
    sendDataToCpp = (data) => {
        try {
            if (!this.controller) throw data;
            console.log(`Data to C++: ${data}`);
            this.controller.stdin.write(`${data}\n`);
        } catch (err) {
            console.error(`Send Data To Cpp error`, err);
            this.sendDataToServer([
                this.cmdFromServer,
                {
                    OK: false,
                    msg: "Needs to start/restart C++",
                },
            ]);
        }
    };
    parseData = (data) => {
        // console.log(`Data: ${data}`);
        return JSON.parse(data);
    };
    cppErrorHandle = (func) => {
        console.log("Handle error...");
        if (!this.controller) {
            //C++ not launched or launch failed
            console.log("C++ is not running or has unexpected error");
            this.sendDataToServer([
                this.cmdFromServer,
                "Needs to start/restart C++",
            ]);
        } else func;
    };
}

const mainSocket = () => {
    const rpiSocket = new RpiSocket();
};

mainSocket();
