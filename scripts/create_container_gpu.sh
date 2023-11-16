sudo docker rm -f diablo-sim

sudo docker run -id --gpus all --name diablo-sim --network host \
	--privileged -v /dev:/dev \
	-e DISPLAY=$DISPLAY \
	-e QT_X11_NO_MITSHM=1 \
	-v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $HOME/Desktop/shared:/shared \
	heisonberg/diablo-sim-env:latest

sudo xhost +
