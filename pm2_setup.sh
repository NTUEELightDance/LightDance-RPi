#pm2 start ~/LightDance-RPi/client/client.py --interpreter python3
# pm2 start startClient.sh
# pm2 start startController.sh
pm2 start lightdance.config.js
pm2 save
echo "FINISH"
