#!/bin/bash

# Install Webots R2023a
wget https://github.com/cyberbotics/webots/releases/download/R2023a/webots_2023a_amd64.deb
sudo apt install ./webots_2023a_amd64.deb -y

# Setup ~/.bashrc
echo 'export WEBOTS_HOME=/usr/local/webots' >> ~/.bashrc
# Setup /etc/ld.so.conf
sudo sh -c "echo '/usr/local/webots/lib/controller' >> /etc/ld.so.conf"

sudo sh -c "ldconfig"

# Setup gdbgui and make diablo-A1 webots proj
cd ./diablo_A1/controllers/diablo_webots/ && make debug && \
wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/gnuplot-cpp/gnuplot-cpp.zip && \
sudo mkdir -p gnuplot && sudo unzip -d gnuplot gnuplot-cpp.zip && cd gnuplot/gnuplot-cpp && sudo make &&\
cd ../../ && sudo cp gnuplot/gnuplot-cpp/gnuplot_i.hpp simulation/

echo 'Finish locol environment setup.'






