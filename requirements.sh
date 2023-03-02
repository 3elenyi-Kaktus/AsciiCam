#!/bin/bash

#prerequisites
sudo apt install -y g++ cmake make git libgtk2.0-dev pkg-config

#clone repo
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

#build the source
mkdir -p build && cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv
make -j $(nproc)

#install the package
sudo make install