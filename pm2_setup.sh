pm2 start startCLI.sh
pm2 start startClient.sh
pm2 start startController.sh
pm2 save
echo "FINISH"
