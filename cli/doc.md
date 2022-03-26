# Lightdance CLI
Command line interface for NTUEE lightdance.

## Launch
``` bash
# At root folder
python cli/cli.py ${dancerName}
```

## Commands
### Configuration Settings
* load
```
Usage: load [controlPath]

Load control JSON files

positional arguments:
  controlPath  Path to control directory.
```
ex: 
```
load
load ./data
```

* setofchannel/setledchannel
```
Usage: setof/ledchannel partName channel

Set OF/LED Channel in JSON

positional arguments:
  partName    partName
  channel     channel
```
ex: 
```
setofchannel Calf_R 10
setledchannel Glove_L_LED 10
```

### Playback Controls
* play
* pause
* stop

### Testing
* oftest/ledtest
```
Usage: of/ledtest channel color alpha

Test OF/LED

positional arguments:
  channel     channel
  color       RGB in HEX or R G B
  alpha       brightness 0~15
```
ex: 
```
oftest 0 0xffffff 10
oftest Calf_R ffffff 10
oftest 0 255 255 255 10

ledtest 0 0xffffff 10
ledtest Glove_L_LED ffffff 10
ledtest 0 255 255 255 10
```

* lightall
```
Usage: lightall color alpha

light all

positional arguments:
  color       RGB in HEX or R G B
  alpha       brightness 0~15
```
* darkall
* traversal # Processing
* red/green/blue

### System
* reboot
* shutdown
* quit
## Startup Script
CLI will run commands in `cli/startup` at start. It's useful for debug routine.
