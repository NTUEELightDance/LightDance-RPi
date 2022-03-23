#TMUX start-up script
echo $DANCERNAME

tmux new-session -d -s lightdance

tmux new-window ~/LightDance-RPi/startCLI.sh
tmux new-window ~/LightDance-RPi/startClient.sh
tmux new-window ~/LightDance-RPi/startController.sh
