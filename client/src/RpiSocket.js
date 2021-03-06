const WebSocket = require("ws");
const { spawn } = require("child_process");
const fs = require("fs");
const path = require("path");
const os = require("os");
const shell = require("shelljs");

class RpiSocket {
    constructor() {
        this.wsClient = null;
        this.controller = null; //c++ child process
        this.cmdFromServer = null;
        this.cppResponse = null;
        this.connected = false;
        this.needToReconnect = true;
        this.musicPlaying = false;
        this.init();
    }
    init = () => {
        this.connectWebsocket();
    };
    connectWebsocket = () => {
        this.wsClient = new WebSocket("ws://localhost:8080");
        if (this.wsClient.readyState !== WebSocket.CONNECTING) {
            setTimeout(() => {
                this.init();
            }, 3000);
            return;
        } else {
            this.listeningServer();
            //if (this.controller !== null) this.listeningCpp();
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
                    name: os.hostname(),
                    OK: true,
                    msg: "Success",
                },
            ]);
        };
        this.wsClient.onerror = (err) => {
            console.log(`Websocket error: ${err.message}`);
        };
        this.wsClient.onmessage = (mes) => {
            const data = mes.data;
            console.log(`Data from server: ${data}`);
            this.parseServerData(data);
        };
        this.wsClient.onclose = (e) => {
            console.log("Websocket client closed.");
            if (!this.musicPlaying && this.controller !== null)
                this.controller.kill(); //若音樂在播而不小心斷線，就不管

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
        this.controller.stdout.on("data", (mes) => {
            const data = mes.toString();
            //console.log(`Data from C++: ${data}`);
            this.parseCppData(data);
        });
    };
    parseServerData = (mes) => {
        const [task, payload] = this.parseData(mes);
        console.log("Command: ", task, "\nPayload: ", payload);
        this.cmdFromServer = task;
        if (this.controller !== null) this.listeningCpp();
        switch (this.cmdFromServer) {
            case "start": {
                //start c++ file
                if (this.controller !== null) {
                    console.log("Killing running C++.");
                    this.controller.kill();
                    console.log("Running C++ is killed");
                }
                this.controller = spawn("../../RPIControler/RPIControler"); //use ./test to test, change to ./controller for real time deployment
                break;
            }
            case "play": {
                //start playing
                const startTime = payload.startTime; //從整首歌的第幾秒播放
                const whenToPlay = payload.whenToPlay; //Rpi從此確切時間開始播放
                this.sendDataToCpp(`PLay ${startTime} ${whenToPlay}`);
                break;
            } //back to server的部分還未確定
            case "pause": {
                //pause playing
                this.controller.kill("SIGINT");
                break;
            }
            case "stop": {
                //stop playing(prepare to start time)
            this.sendDataToCpp("RPTStop");  //another SIG
                break;
            }
            case "load": {
                //call cpp to load play file.json
                this.sendDataToCpp("Load");
                break;
            }
            case "terminate": {
                //terminate c++ file
                if (this.controller !== null) this.controller.kill("SIGKILL");
                this.sendDataToServer([
                    "terminate",
                    {
                        OK: true,
                        msg: "terminated",
                    },
                ]);
                break;
            }
            case "lightCurrentStatus": {
                this.sendDataToCpp([
                    //data structure待訂，payload會有一個.json

                ]);
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

                if (this.controller !== null) this.controller.kill("SIGKILL");
                this.controller = null;
                this.cmdFromServer = null;
                this.cppResponse = null;
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
                                    msg: "upload success",
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
                                    msg: "upload success",
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
                shell.exec("shutdown -h 0");
                break;
            }
            case "reboot": {
                //reopen Rpi computer
                shell.exec("reboot -h 0");
                break;
            }
            case "sync": {
                //payload 至少會有時間，將Rpi的時間與電腦同步\

                break;
            }
            case "boardInfo":{
                this.sendDataToServer([
                    "boardInfo",
                    {
                        //send boardInfo while connected to server
                        name: os.hostname(),
                        OK: true,
                        msg: "Success",
                    },
                ]);
                break;
            }
        }
    };
    parseCppData = (mes) => {
        //const [task, payload] = this.parseData(mes.toString());
        const message = mes.toString().split(" ");
        for (let i = 0; i < message.length; i++) message[i] = message[i].trim();
        this.cppResponse = message[0];
        console.log("Controller response: ", message[0], "Success: ", message[1] === '1', message[1]);
        switch (this.cppResponse) {
            case "start": {
                this.sendDataToServer([
                    "start",
                    {
                        OK: message[1] === "1",
                        message: `start ${
                            message[1] === "1" ? "success" : "failed"
                        }`,
                    },
                ]);
                break;
            }
            case "Load": {
                this.sendDataToServer([
                    "load",
                    {
                        OK: message[1] === "1",
                        message: `load ${
                            message[1] === "1" ? "success" : "failed"
                        }`,
                    },
                ]);
                break;
            }
            case "play": {
                this.sendDataToServer([
                    //暫定，可能會再改
                    "play",
                    {
                        OK: message[1] === "1",
                        message: `play ${
                            message[1] === "1" ? "success" : "failed"
                        }`,
                    },
                ]);
                this.musicPlaying = message[1] === "1";
                break;
            }
            case "pause": {
                this.sendDataToServer([
                    "pause",
                    {
                        OK: message[1] === "1",
                        message: `pause ${
                            message[1] === "1" ? "success" : "failed"
                        }`,
                    },
                ]);
                break;
            }
            case "stop": {
                this.sendDataToServer([
                    "stop",
                    {
                        OK: message[1] === "1",
                        message: `stop ${
                            message[1] === "1" ? "success" : "failed"
                        }`,
                    },
                ]);
                break;
            }
            case "lightCurrentStatus": {
                this.sendDataToServer([
                    "lightCurrentStatus",
                    {
                        OK: message[1] === "1",
                        message: `lightCurrentStatus ${
                            message[1] === "1" ? "success" : "failed"
                        }`,
                    },
                ]);
                break;
            }
        }
    };
    sendDataToServer = (data) => {
        this.wsClient.send(JSON.stringify(data));
    };
    sendDataToCpp = (data) => {
        console.log(`Data to C++: ${data}`);
        this.controller.stdin.write(`${data}\n`);
    };
    parseData = (data) => {
        console.log(`Data: ${data}`);
        return JSON.parse(data);
    };
    cppErrorHandle = (func) => {
        console.log("Handle error...");
        if (this.controller === null) {
            //C++ not launched or launch failed
            console.log("C++ is not running or has unexpected error");
            this.sendDataToServer(["Error", "Needs to start/restart C++"]);
        } else func;
    };
}

const mainSocket = () => {
    const rpiSocket = new RpiSocket();
};

mainSocket();