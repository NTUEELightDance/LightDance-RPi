SERVER_IP=$1

sed -i "s!\(SERVER_IP = \)\".*\"!\1\"${SERVER_IP}\"!g" ws_client/config.py
sudo systemctl restart client.service
