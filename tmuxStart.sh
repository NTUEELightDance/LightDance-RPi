#TMUX start-up script
echo $DANCERNAME

tmux new-session -d -s lightdance

new-window ~/LightDance-RPi/startCLI.sh
new-window ~/LightDance-RPi/startClient.sh
new-window ~/LightDance-RPi/startController.sh
