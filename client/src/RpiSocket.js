const WebSocket = require("ws");
const { spawn } = require("child_process");
const fs = require("fs");
const path = require("path");
const os = require("os");
const ip = require("ip");
const shell = require("shelljs");
const e = require("express");

class RpiSocket {
    constructor(){
        this.wsClient = null;
        this.controller = null;  //c++ child process
        this.cmdFromServer = null;
        this.cppResponse = null;
        this.musicPlaying = false;
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
            this.sendDataToServer(["boardInfo", {  //send boardInfo while connected to server
                name: os.hostname(),
                OK: true,
                msg: "Success"
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
            }  //back to server的部分還未確定
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
                if(this.controller !== null) this.controller.kill("SIGKILL");
                this.sendDataToServer([
                    "terminate",
                    {
                        OK: true,
                        msg: "terminated"
                    }
                ]);
                break;
            }
            case "lightCurrentStatus":{
                this.cppErrorHandle(this.sendDataToCpp([
                    //data structure待訂，payload會有一個.json
                ]));
                break;
            }
            //above are commands sending to clientApp(controller.cpp)
            case "kick": {  //reconnect to websocket server
                this.wsClient.close();
                this.wsClient.onclose = (e) => {
                    console.log("Websocket client is closed.")
                }
                this.connectWebsocket();

                if (this.controller !== null) this.controller.kill("SIGKILL");
                this.controller = null;
                this.cmdFromServer = null;
                this.cppResponse = null;
                this.musicPlaying = false;
                break;
            }
            case "uploadControl":{  //load timeline.json to local storage(./current)
                //console.log(payload);
                if (!fs.existsSync(path.join(__dirname, "../../data"))) fs.mkdirSync(path.join(__dirname, "../../data"));
                fs.writeFile(
                    path.join(__dirname, "../../data/control.json"),
                    JSON.stringify(payload),
                    (err) => {
                        if (err){
                            console.log("Upload control file failed.");
                            this.sendDataToServer([
                                "uploadControl",
                                {
                                    OK: false,
                                    msg: "upload failed"
                                }
                            ]);
                            throw err;
                        }
                        else {
                            console.log("Upload control file success.");
                            this.sendDataToServer([
                                "uploadControl",
                                {
                                    OK: true,
                                    msg: "upload success"
                                }
                            ]);
                        }
                    }
                );
                break;
            }
            case "uploadLED":{  //load led picture files to ./control/LED
                if (!fs.existsSync(path.join(__dirname, "../../asset"))) fs.mkdir(fs.existsSync(path.join(__dirname, "../../asset")));
                for (let i = 0; i < payload.length; i++){  //led files
                    fs.writeFile(
                        path.join(__dirname, "../../asset", payload[i].name) + ".json",
                        JSON.stringify(payload[i].data),
                        (err) => {
                            if (err){
                                console.log(`Upload ${payload[i].name} failed.`);
                                this.sendDataToServer([
                                    "uploadLED",
                                    {
                                        OK: false,
                                        msg: "upload failed"
                                    }
                                ]);
                                throw err;
                            }
                            else {
                                console.log(`Upload ${payload[i].name} success.`);
                                this.sendDataToServer([
                                    "uploadLED",
                                    {
                                        OK: true,
                                        msg: "upload success"
                                    }
                                ]);
                            }
                        }
                    );
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
            case "sync":{  //payload 至少會有時間，將Rpi的時間與電腦同步\

                break;
            }
        }
    }
    parseCppData = (mes) => {
        //const [task, payload] = this.parseData(mes.toString());
        const message = mes.toString().split(" ");
        this.cppResponse = message[0];
        switch (this.cppResponse){
            case "start":{
                this.sendDataToServer([
                    "start",
                    {
                        OK: (message[1] === "1") ? true : false,
                        message: `start ${(message[1] === "1") ? "success" : "failed"}`
                    }
                ]);
                break;
            }
            case "load":{
                this.sendDataToServer([
                    "load",
                    {
                        OK: (message[1] === "1") ? true : false,
                        message: `load ${(message[1] === "1") ? "success" : "failed"}`
                    }
                ]);
                break;
            }
            case "play":{
                this.sendDataToServer([  //暫定，可能會再改
                    "play",
                    {
                        OK: (message[1] === "1") ? true : false,
                        message: `play ${(message[1] === "1") ? "success" : "failed"}`
                    }
                ]);
                this.musicPlaying = (message[i] === "1");
                break;
            }
            case "pause":{
                this.sendDataToServer([
                    "pause",
                    {
                        OK: (message[1] === "1") ? true : false,
                        message: `pause ${(message[1] === "1") ? "success" : "failed"}`
                    }
                ]);
                break;
            }
            case "stop":{
                this.sendDataToServer([
                    "stop",
                    {
                        OK: (message[1] === "1") ? true : false,
                        message: `stop ${(message[1] === "1") ? "success" : "failed"}`
                    }
                ]);
                break;
            }
            case "lightCurrentStatus":{
                this.sendDataToServer([
                    "lightCurrentStatus",
                    {
                        OK: (message[1] === "1") ? true : false,
                        message: `lightCurrentStatus ${(message[1] === "1") ? "success" : "failed"}`
                    }
                ]);
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
