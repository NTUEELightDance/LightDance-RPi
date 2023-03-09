# Create root content folder
sudo mkdir -p /lightdance
sudo mkdir -p /lightdance/config
sudo mkdir -p /lightdance/data

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
cd ..

# Copy services
sudo cp -r system/* /etc/systemd/system/
sudo systemctl daemon-reload

# Copy configuration files
sudo cp ws_client/config.py /lightdance/config/
