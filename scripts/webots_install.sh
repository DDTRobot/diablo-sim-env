#!/bin/bash

# Install Webots and dependencies
sudo apt-get update && sudo apt-get upgrade -y && sudo apt-get install --yes wget bzip2 xvfb gnuplot make&& \
wget https://github.com/cyberbotics/webots/releases/download/R2023a/webots-R2023a-x86-64.tar.bz2 && \
tar xjf webots-*.tar.bz2 && rm webots-*.tar.bz2

# Install Webots runtime dependencies
wget https://raw.githubusercontent.com/cyberbotics/webots/master/scripts/install/linux_runtime_dependencies.sh && \
chmod +x linux_runtime_dependencies.sh && ./linux_runtime_dependencies.sh && rm ./linux_runtime_dependencies.sh 

# Setup gdbgui and make diablo-A1 webots proj
cd ~/diablo_A1/controllers/diablo_webots/ && make debug && \
wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/gnuplot-cpp/gnuplot-cpp.zip && \
mkdir -p /opt/gnuplot && unzip -d /opt/gnuplot gnuplot-cpp.zip && cd /opt/gnuplot/gnuplot-cpp && make &&\
cp gnuplot_i.hpp ~/diablo_A1/controllers/diablo_webots/simulation
  


# Setup ~/.bashrc
echo 'export TERM=xterm-256color\n\
export PATH="$PATH:$HOME/.local/bin"\n\
export WEBOTS_HOME=/usr/local/webots\n'\
>> ~/.bashrc

# Setup /etc/ld.so.conf
echo '/usr/local/webots/lib/controller\n'\
>> /etc/ld.so.conf

ldconfig


