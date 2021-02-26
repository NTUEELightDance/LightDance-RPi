const WebSocket = require("ws");
const { spawn } = require("child_process");
const fs = require("fs");
const path = require("path");
const boardInfo = require("../board_config.json");
const os = require("os");
const ip = require("ip");
const shell = require("shelljs");

class RpiSocket {
    constructor(){
        this.wsClient = null;
        this.controller = null;  //c++ child process
        this.cmdFromServer = null;
        this.cppResponse = null;
        this.init();
    }
    init = () => {
        this.connectWebsocket();
    }
    connectWebsocket = () => {
        this.wsClient = new WebSocket("ws://localhost:4000");
        this.wsClient.addEventListener("error", (err) => {
            if (err.target.readyState !== 1){
                console.log("Websocket server has some error, reconnecting to Websocket server...");
                setTimeout(() => this.connectWebsocket(), 3000);
            }
        });
        
        this.listeningServer();
    }
    listeningServer = () => {
        this.wsClient.onopen = () => {
            console.log("Websocket connected.");
            this.sendDataToServer(["boardInfo", {
                name: boardInfo.name,
                board_type: boardInfo.board_type,
                hostname: os.hostname(),
                ip: ip.address()
            }]);
        }
        this.wsClient.onmessage = (mes) => {
            const data = mes.data;
            console.log(`Data from server: ${data}`);
            this.parseServerData(data);
        }
    }
    listeningCpp = () => {
        this.controller.stdout.on("data", (mes) => {
            const data = mes.toString();
            console.log(`Data from C++: ${data}`);
            this.parseCppData(data);
        })
    }
    parseServerData = (mes) => {
        const [task, payload] = this.parseData(mes);
        this.cmdFromServer = task;
        if (this.controller !== null) this.listeningCpp();
        switch (this.cmdFromServer){
            case "start":{  //start c++ file
                if (this.controller !== null){
                    console.log("Killing running C++.");
                    this.controller.kill();
                    console.log("Running C++ is killed");
                }
                this.controller = spawn("../test");  //use ./test to test, change to ./controller for real time deployment
                break;
            }
            case "play":{  //start playing
                const startTime = payload.startTime;  //從整首歌的第幾秒播放
                const whenToPlay = payload.whenToPlay;  //Rpi從此確切時間開始播放
                this.cppErrorHandle(this.sendDataToCpp(`RPIPLay ${startTime} ${whenToPlay}`));
                break;
            }
            case "pause":{  //pause playing
                this.controller.kill("SIGINT");
                break;
            }
            case "stop":{  //stop playing(prepare to start time)
                this.cppErrorHandle(this.sendDataToCpp("RPTStop"));
                break;
            }
            case "load":{  //call cpp to load play file.json
                this.cppErrorHandle(this.sendDataToCpp("RPILoad"));
                break;
            }
            case "terminate":{  //terminate c++ file
                this.controller.kill("SIGKILL");
                break;
            }
            //above are commands sending to clientApp(controller.cpp)
            case "kick": {

                break;
            }
            case "uploadControl":{  //load timeline.json to local storage(./current)
                console.log(payload);
                if (!fs.existsSync(path.join(__dirname, "./control"))) fs.mkdirSync(path.join(__dirname, "./control"));
                fs.writeFileSync(
                    path.join(__dirname, "./control/timeline.json"),
                    JSON.stringify(payload)
                );
                console.log("Timeline file saved.")
                break;
            }
            case "uploadLED":{  //load led picture files to ./control/LED
                if (!fs.existsSync(path.join(__dirname, "./control/LED"))) fs.mkdir(fs.existsSync(path.join(__dirname, "./control/LED")));
                for (let i = 0; i < payload.length; i++){  //led files
                    //const controlLEDFile = JSON.stringify(payload[i]);
                    fs.writeFileSync(
                        path.join(__dirname, "./control/LED", payload[i].name) + ".json",
                        JSON.stringify(payload[i].data),
                    );
                    console.log(`${payload[i].name} saved.`);
                }
                break;
            }
            case "shutDown":{  //shut down Rpi computer
                shell.exec("shutdown -h 0");
                break;
            }
            case "reboot":{  //reopen Rpi computer
                shell.exec("reboot -h 0");
                break;
            }
        }
    }
    parseCppData = (mes) => {
        //const [task, payload] = this.parseData(mes.toString());
        const message = mes.toString().split(" ");
        this.cppResponse = message[0];
        switch (this.cppResponse){
            case "Success":{
                this.sendDataToServer(["Success", this.cmdFromServer]);
                break;
            }
            case "Error":{
                this.sendDataToServer(["Error", this.cmdFromServer]);
                break;
            }
            case "Other":{
                console.log(message);
                break;
            }
        }
    }
    sendDataToServer = (data) => {
        this.wsClient.send(JSON.stringify(data));
    }
    sendDataToCpp = (data) => {
        console.log(`Data to C++: ${data}`);
        this.controller.stdin.write(`${data}\n`);
    }
    parseData = (data) => {
        //return JSON.parse(data);
        console.log(`Data: ${data}`);
        return JSON.parse(data);
    }
    cppErrorHandle = (func) => {
        console.log("Handle error...");
        if (this.controller === null){  //C++ not launched or launch failed
            console.log("C++ is not running or has unexpected error");
            this.sendDataToServer(["Error", "Needs to start/restart C++"]);
        }
        else func;
    }
}

const mainSocket = () => {
    const rpiSocket = new RpiSocket;
}

mainSocket();
