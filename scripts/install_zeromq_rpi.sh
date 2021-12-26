# Download necessary packages
sudo apt-get install libtool pkg-config build-essential autoconf automake

# Installation of libsodium package
curl https://download.libsodium.org/libsodium/releases/libsodium-1.0.18.tar.gz --output libsodium-1.0.18.tar.gz
tar -zxvf libsodium-1.0.18.tar.gz
cd libsodium-1.0.18/
sudo ./configure
sudo make
sudo make install
cd ../


# Installation of zeromq
wget https://archive.org/download/zeromq_4.1.4/zeromq-4.1.4.tar.gz
tar -zxvf zeromq-4.1.4.tar.gz
cd zeromq-4.1.4/
sudo ./configure
sudo make
sudo make install
cd /usr/include/
sudo wget https://raw.githubusercontent.com/zeromq/cppzmq/master/zmq.hpp

# Remove no use folders and files
cd /home/pi/LightDance-RPi/scripts
rm libsodium-1.0.18.tar.gz
sudo rm -rf libsodium-1.0.18
rm zeromq-4.1.4.tar.gz
sudo rm -rf zeromq-4.1.4

# set include file
sudo echo /etc/ld.so.conf >> /usr/local/lib/libzmq.so.5
sudo ldconfig