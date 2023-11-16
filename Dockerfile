ARG BASE_IMAGE=nvidia/cuda:11.8.0-base-ubuntu20.04
FROM ${BASE_IMAGE} AS downloader


# Disable dpkg/gdebi interactive dialogs
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y && apt-get install --yes wget bzip2 && rm -rf /var/lib/apt/lists/ && \
 wget https://github.com/cyberbotics/webots/releases/download/R2023a/webots-R2023a-x86-64.tar.bz2 && \
 tar xjf webots-*.tar.bz2 && rm webots-*.tar.bz2

FROM ${BASE_IMAGE}

# Disable dpkg/gdebi interactive dialogs
ENV DEBIAN_FRONTEND=noninteractive

# Install Webots runtime dependencies
RUN apt-get update && apt-get install --yes wget xvfb && rm -rf /var/lib/apt/lists/ && \
  wget https://raw.githubusercontent.com/cyberbotics/webots/master/scripts/install/linux_runtime_dependencies.sh && \
  chmod +x linux_runtime_dependencies.sh && ./linux_runtime_dependencies.sh && rm ./linux_runtime_dependencies.sh 

# Install Webots
WORKDIR /usr/local
COPY --from=downloader /webots /usr/local/webots/
# ENV QTWEBENGINE_DISABLE_SANDBOX=1
ENV WEBOTS_HOME /usr/local/webots
ENV PATH /usr/local/webots:${PATH}

# Enable OpenGL capabilities
ENV NVIDIA_DRIVER_CAPABILITIES graphics,compute,utility
ENV NVIDIA_VISIBLE_DEVICES all

#Add diablo_A1 sim proj
RUN mkdir -p /opt/webots-sim-projects/sim-webots/diablo_A1
ADD diablo_A1 /opt/webots-sim-projects/sim-webots/diablo_A1

RUN apt-get install --yes gnuplot wget make && rm -rf /var/lib/apt/lists/ &&\
  cd /opt/webots-sim-projects/sim-webots/diablo_A1/controllers/diablo_webots/ && make debug && \
  wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/gnuplot-cpp/gnuplot-cpp.zip && \
  mkdir -p /opt/gnuplot && unzip -d /opt/gnuplot gnuplot-cpp.zip && cd /opt/gnuplot/gnuplot-cpp && make &&\
  cp gnuplot_i.hpp /opt/webots-sim-projects/sim-webots/diablo_A1/controllers/diablo_webots/simulation
  
# Setup gdbgui env
RUN export PATH="$PATH:$HOME/.local/bin"


# setup ~/.bashrc
RUN echo 'export TERM=xterm-256color\n\
export PATH="$PATH:$HOME/.local/bin"\n\
export WEBOTS_HOME=/usr/local/webots\n'\
>> ~/.bashrc

# Setup /etc/ld.so.conf
RUN echo '/usr/local/webots/lib/controller\n'\
>> /etc/ld.so.conf
RUN ldconfig

# Finally open a bash command to let the user interact
CMD ["/bin/bash"]

