HOSTNAME=$1
OLD_HOSTNAME=$(head -n 1 /etc/hostname)

sudo sed -i "s!${OLD_HOSTNAME}!${HOSTNAME}!g" /etc/hostname
sudo sed -i "s!${OLD_HOSTNAME}!${HOSTNAME}!g" /etc/hosts
