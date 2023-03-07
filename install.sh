# Build controller
cd controller
mkdir -p build && cd build
cmake .. && make -j4 install
cd ../..

# Install controller binaries to /usr/local/bin
cd controller
sudo rm -rf /usr/local/bin/*
sudo cp -r bin/* /usr/local/bin/
sudo chown root /usr/local/bin/*
sudo chmod u+s /usr/local/bin/*

# Bui