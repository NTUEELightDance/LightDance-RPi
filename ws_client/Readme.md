# Websocket client on RPi

## Protocols of RPi to control server

### on_connect

* send MAC address to control server when connected

MAC address example : F8:5E:A0:2F:B7:13


```json
{
    "command":"boardInfo",
    "status": 0,
    "payload": 
    {
        "type":"RPi",
        "info":{
            "macaddr": str
        }
    }
}
```

### after executing command

* command : the command RPi just executed/got from control server
* status : return value of the execution (0 for success)
* message : return message or error message from execution

```json
{
    "command": str,
    "status": int,
    "payload": {
        "type": "RPi",
        "info":{
            "message": str
        }
    }
}
```

## Protocols of control server to RPi

```json
{
    "action": str,
    "payload": []
}

// examples
{
	"action": "command",
	"payload": [
	    "playerctl",
		"play",
		"10"
	]
}

{
	"action": "upload",
	"payload": [
	    control.json,
	    OF.json,
	    LED.json
	]
}

```
* **control.json** is the pin table of LEDs and OFs

## Command type for control server to send

| command       | action        | payload       | flags            |
| ------------- |:-------------:|:-------------:|:--------------:|
| upload        | "upload"      | [control.json, OF.json, LED.json]         |
| play          | "command"     | ["playerctl", "play", start time : str]   | -s -d |
| pause         | "command"     | ["playerctl", "pause"]    |
| stop          | "command"     | ["playerctl", "stop"]     |
| restart       | "command"     | ["playerctl", "restart"]  |
| quit          | "command"     | ["playerctl", "quit"]     |
| list          | "command"     | ["list"]                  |
| LEDtest       | "command"     | ["ledtest", flags...]               | --rgb --hex -a| 
| OFtest        | "command"     | ["oftest", flags...]                | --rgb --hex -a|

### meaning of flags
#### play
|flag       |meaning                |
| -----     |:---------------------:|
|-s --sec   | set start time unit to **second**  |
|-d --delay | set delay before play |

**if not specified by flag**
* start from 0
* time unit is millisecond


#### LEDtest OFtest
|flag   |meaning            |
| ----- |:-----------------:|
|--rgb  | set r,g,b value   |
|--hex  | set hex color code|
|-a     | set alpha value   |


## Example of json files

### control.json

```json
{
  "fps": 30,
  "OFPARTS": {
    "of1": 0,
    "of2": 1,
    "of3": 2,
    "of4": 25,
    "of5": 5,
    "of6": 6,
    "of7": 7,
    "of8": 17,
    "of9": 19,
    "of10": 21,
    "of11": 23,
    "of12": 9,
    "of13": 13,
    "of14": 14,
    "of15": 3,
    "of16": 20,
    "of17": 4,
    "of18": 11,
    "of19": 18,
    "of20": 12
  },
  "LEDPARTS": {
    "led1": {
      "id": 0,
      "len": 4
    },
    "led2": {
      "id": 1,
      "len": 4
    },
    "led3": {
      "id": 2,
      "len": 4
    }
  }
}
```

### LED.json
```json
{
  "led1": [
    {
      "start": 0,
      "fade": true,
      "status": [
          [40, 0, 0, 10],
          [40, 0, 0, 10],
          [40, 0, 0, 10],
          [40, 0, 0, 10]
      ]
    },
    {
      "start": 150,
      "fade": true,
      "status": [
          [0, 40, 0, 10],
          [0, 40, 0, 10],
          [0, 40, 0, 10],
          [0, 40, 0, 10]
      ]
    },
    {
      "start": 300,
      "fade": false,
      "status": [
          [0, 0, 40, 10],
          [0, 0, 40, 10],
          [0, 0, 40, 10],
          [0, 0, 40, 10]
      ]
    }
  ],
  "led2": [
    {
      "start": 0,
      "fade": true,
      "status": [
        [0, 0, 0, 10],
        [0, 0, 0, 10],
        [0, 0, 0, 10],
        [0, 0, 0, 10]
      ]
    },
    {
      "start": 150,
      "fade": true,
      "status": [
        [0, 0, 40, 10],
        [0, 0, 40, 10],
        [0, 0, 40, 10],
        [0, 0, 40, 10]
      ]
    },
    {
      "start": 300,
      "fade": false,
      "status": [
        [0, 0, 0, 10],
        [0, 0, 0, 10],
        [0, 0, 0, 10],
        [0, 0, 0, 10]
      ]
    }
  ],
  "led3": [
    {
      "start": 0,
      "fade": false,
      "status": [
        [40, 40, 40, 10],
        [40, 40, 40, 10],
        [40, 40, 40, 10],
        [40, 40, 40, 10]
      ]
    },
    {
      "start": 60,
      "fade": false,
      "status": [
        [0, 0, 0, 10],
        [0, 0, 0, 10],
        [0, 0, 0, 10],
        [0, 0, 0, 10]
      ]
    },
    {
      "start": 120,
      "fade": false,
      "status": [
        [40, 40, 40, 10],
        [40, 40, 40, 10],
        [40, 40, 40, 10],
        [40, 40, 40, 10]
      ]
    }
  ]
}
```

### OF.json

```json
[
  {},
  {
    "start": 0,
    "fade": true,
    "status": {
      "of1": [30, 30, 30, 10],
      "of2": [30, 30, 30, 10],
      "of3": [30, 30, 30, 10],
      "of4": [30, 30, 30, 10],
      "of5": [30, 30, 30, 10],
      "of6": [30, 30, 30, 10],
      "of7": [30, 30, 30, 10],
      "of8": [30, 30, 30, 10],
      "of9": [30, 30, 30, 10],
      "of10": [30, 30, 30, 10],
      "of11": [30, 30, 30, 10],
      "of12": [30, 30, 30, 10],
      "of13": [30, 30, 30, 10],
      "of14": [30, 30, 30, 10],
      "of15": [30, 30, 30, 10],
      "of16": [30, 30, 30, 10],
      "of17": [30, 30, 30, 10],
      "of18": [30, 30, 30, 10],
      "of19": [30, 30, 30, 10],
      "of20": [30, 30, 30, 10]
    }
  },
  {
    "start": 10000,
    "fade": true,
    "status": {
      "of1": [30, 30, 30, 0],
      "of2": [30, 30, 30, 0],
      "of3": [30, 30, 30, 0],
      "of4": [30, 30, 30, 0],
      "of5": [30, 30, 30, 0],
      "of6": [30, 30, 30, 0],
      "of7": [30, 30, 30, 0],
      "of8": [30, 30, 30, 0],
      "of9": [30, 30, 30, 0],
      "of10": [30, 30, 30, 0],
      "of11": [30, 30, 30, 0],
      "of12": [30, 30, 30, 0],
      "of13": [30, 30, 30, 0],
      "of14": [30, 30, 30, 0],
      "of15": [30, 30, 30, 0],
      "of16": [30, 30, 30, 0],
      "of17": [30, 30, 30, 0],
      "of18": [30, 30, 30, 0],
      "of19": [30, 30, 30, 0],
      "of20": [30, 30, 30, 0]
    }
  }
]

```