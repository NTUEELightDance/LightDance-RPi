brew install zeromq
git clone https://github.com/zeromq/cppzmq.git
mkdir build
cd build
cmake ..
sudo make -j4 install