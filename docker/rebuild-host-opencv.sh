#!/bin/bash

# Download, build, and pack opencv for jevois
ma=4
mi=10
pa=0

# Install boost first and then decide on python version:
sudo apt install -y libboost-all-dev

pyver="OOPS"
if [ -f "/usr/lib/x86_64-linux-gnu/libboost_python312.so" ]; then pyver="3.12";
elif [ -f "/usr/lib/x86_64-linux-gnu/libboost_python38.so" ]; then pyver="3.8";
elif [ -f "/usr/lib/x86_64-linux-gnu/libboost_python3-py37.so" ]; then pyver="3.7";
elif [ -f "/usr/lib/x86_64-linux-gnu/libboost_python-py36.so" ]; then pyver="3.6";
elif [ -f "/usr/lib/x86_64-linux-gnu/libboost_python-py35.so" ]; then pyver="3.5"; fi
if [ $pyver = "OOPS" ]; then echo "Cannot find libboost_python py35, py36, py37, 38 or 312 -- ABORT"; exit 5; fi

pkgrel="1"
uburel=`lsb_release -rs`
arch=`dpkg --print-architecture`

####################################################################################################
# Compiler packages and dependencies
if [ ${uburel} = "15.10" ]; then
    compilers="gcc-5 g++-5 gfortran-5"
    qt4lib="libqt4-dev"
elif [ ${uburel} = "16.04" ]; then
    # for 16.04 we need a ppa:
    compilers="gcc-7 g++-7 gfortran-7"
    if [ ! -x /usr/bin/gcc-7 -o ! -x /usr/bin/g++-7 -o ! -x /usr/bin/gfortran-7 ]; then
	    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test # needed for gcc-6 on 16.04
	    sudo apt-get update
    fi
    qt4lib="libqt4-dev"
elif [ ${uburel} = "17.04" ]; then
    compilers="gcc-7 g++-7 gfortran-7"
    qt4lib="libqt4-dev libavresample-dev"
elif [ ${uburel} = "17.10" ]; then
    compilers="gcc-7 g++-7 gfortran-7"
    qt4lib="libqt4-dev libavresample-dev"
elif [ ${uburel} = "18.04" -o ${uburel} = "19.04" ]; then
    compilers="gcc-8 g++-8 gfortran-8"
    qt4lib="libqt4-dev libavresample-dev"
elif [ ${uburel} = "20.04" ]; then
    compilers="gcc-10 g++-10 gfortran-10"
    qt4lib="libavresample-dev" # no more qt4 on 20.04; we already have qt5-default in the list below
elif [ ${uburel} = "24.04" ]; then
    compilers="gcc-10 g++-10 gfortran-10" # we stick to gcc-10 for jevois, jevois-pro moved to gcc-14
    qt4lib=""
else
    echo "Unsupported ubuntu version -- ABORT"
    exit 1
fi

packages=( build-essential python-is-python3 ${compilers} cmake libboost-all-dev autoconf libeigen3-dev libgtk2.0-dev
    libdc1394-dev libjpeg-dev libpng-dev libtiff5-dev libavcodec-dev libavformat-dev libxine2-dev libgstreamer1.0-dev
    libgstreamer-plugins-base1.0-dev libv4l-dev libtbb-dev ${qt4lib} libfaac-dev libmp3lame-dev
    libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils unzip
    python${pyver}-dev python3-numpy python3-pip libgtk-3-dev libopenblas-base-dev libturbojpeg checkinstall
    protobuf-compiler libprotobuf-dev libtesseract-dev tesseract-ocr-all libleptonica-dev libatlas-base-dev
    pylint libopenjp2-7-dev libopenjpip7 )

# Install packages unless they are already installed:
failed=""
for pack in "${packages[@]}"; do
    if `dpkg --list | grep -q $pack`; then
        echo "$pack already installed"
    else
        sudo apt-get --assume-yes install $pack
        if [ $? -ne 0 ]; then failed="$failed $pack"; fi
    fi
done

if [ "X$failed" != "X" ]; then
    echo
    echo
    echo "### WARNING: Some packages failed to install #####" 
    echo
    echo "Install failed for: $failed"
    echo
    echo
    echo "This will likely be a problem for the rest of this script."
    echo "You should try to install them using synaptic or other."
    read -p "Abort this script now [Y/n]? "
    if [ "X$REPLY" != "Xn" ]; then exit 2; fi
fi

# Create the dependencies from the packages:
printf -v pkgdeps '%s,' "${packages[@]}"
pkgdeps="${pkgdeps%,}"

# Fix compilation error with libjpeg-turbo8
if [ ! -f /usr/lib/x86_64-linux-gnu/libturbojpeg.so ]; then
    sudo ln -s /usr/lib/x86_64-linux-gnu/libturbojpeg.so.0.?.0 /usr/lib/x86_64-linux-gnu/libturbojpeg.so
fi

if [ ${uburel} = "16.04" ]; then
    # And we need cmake to use gcc6
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 50 --slave /usr/bin/g++ g++ /usr/bin/g++-5 --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-5
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 100 --slave /usr/bin/g++ g++ /usr/bin/g++-7 --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-7
    sudo update-alternatives --set gcc /usr/bin/gcc-7
elif [ ${uburel} = "18.04" -o ${uburel} = "19.04" ]; then
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-8
    sudo update-alternatives --set gcc /usr/bin/gcc-8
else
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 --slave /usr/bin/g++ g++ /usr/bin/g++-10 --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-10
    sudo update-alternatives --set gcc /usr/bin/gcc-10
fi

####################################################################################################
# Opencv installation and packing
ver=${ma}.${mi}.${pa}

sudo rm -rf opencv-${ver}
sudo rm -rf /usr/share/jevois-opencv-${ver}
mkdir opencv-${ver}
cd opencv-${ver}
root=`pwd`

wget https://github.com/opencv/opencv/archive/${ver}.tar.gz
tar zxvf ${ver}.tar.gz
rm -f ${ver}.tar.gz

wget https://github.com/opencv/opencv_contrib/archive/${ver}.tar.gz
tar zxvf ${ver}.tar.gz
rm -f ${ver}.tar.gz

# Run cmake to generate the makefile, install to /usr:
cd opencv-${ver}
mkdir build
cd build


cmake \
    -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/share/jevois-opencv-${ver} \
    -D CPACK_PACKAGING_PREFIX=/usr/share/jevois-opencv-${ver} \
    -D CPACK_SET_DESTDIR=ON \
    -D OPENCV_VCSVERSION="${ver}-${pkgrel}ubuntu${uburel}" \
    -D PYTHON_DEFAULT_EXECUTABLE=/usr/bin/python${pyver} \
    -D PYTHON_EXECUTABLE=/usr/bin/python${pyver} \
    -D PYTHON2_EXECUTABLE=/usr/bin/python2.7 \
    -D BUILD_OPENCV_PYTHON3=ON \
    \
    -D BUILD_TESTS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    \
    -D WITH_EIGEN=ON -D BUILD_opencv_python2=OFF -D BUILD_JAVA=OFF -D BUILD_NEW_PYTHON_SUPPORT=ON \
    -D ENABLE_FAST_MATH=1 -D WITH_OPENMP=ON -D ENABLE_CXX11=ON \
    -D OPENCV_ENABLE_NONFREE=ON \
    \
    -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_QT=OFF -D WITH_OPENGL=OFF -D WITH_OPENCL=OFF \
    -D WITH_VTK=OFF -D WITH_CUDA=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF -D BUILD_EXAMPLES=OFF -D INSTALL_C_EXAMPLES=OFF \
    -D OPENCV_ENABLE_NONFREE=ON \
    -D OPENCV_EXTRA_MODULES_PATH=${root}/opencv_contrib-${ver}/modules \
    -D ENABLE_PRECOMPILED_HEADERS=OFF \
    -D CPACK_BINARY_DEB=ON -D CPACK_BINARY_STGZ=OFF -D CPACK_BINARY_TZ=OFF -D CPACK_BINARY_TGZ=OFF \
    ..
      
# build
make -j

# Description
cat > description-pak <<EOF
OpenCV ${ver} configured for use with JeVois smart machine vision
EOF

# create post install and remove scripts because --addso does not work well:
cat > postinstall-pak <<EOF
#!/bin/sh
echo "/usr/share/jevois-opencv-${ver}/lib" > /etc/ld.so.conf.d/jevois-opencv.conf
echo "/usr/share/jevois-opencv-${ver}/lib/python${pyver}/dist-packages/cv2/python-${pyver}" >> /etc/ld.so.conf.d/jevois-opencv.conf
ldconfig
# Fix compilation error with libjpeg-turbo8
if [ ! -f /usr/lib/x86_64-linux-gnu/libturbojpeg.so ]; then
    sudo ln -s /usr/lib/x86_64-linux-gnu/libturbojpeg.so.0.?.0 /usr/lib/x86_64-linux-gnu/libturbojpeg.so
fi
EOF
chmod a+x postinstall-pak
cat > postremove-pak<<EOF
#!/bin/sh
/bin/rm -f /etc/ld.so.conf.d/jevois-opencv.conf
ldconfig
EOF
chmod a+x postremove-pak


# install it for the next steps
sudo make install
sudo mkdir -p /usr/share/jevoispro-opencv-${ver}/share/licenses/opencv4 # needed by checkinstall...

# pack it
sudo checkinstall \
     --type debian \
     --pkgname jevois-opencv \
     --provides jevois-opencv \
     --pkglicense "3-clause BSD" \
     --maintainer "Laurent Itti \\<jevois.org@gmail.com\\>" \
     --nodoc \
     --arch "${arch}" \
     --pkgsource="https://github.com/opencv/opencv/archive/${ver}.tar.gz" \
     --pkgaltsource="https://github.com/opencv/opencv/tree/${ver}" \
     --pkgversion "${ver}" \
     --reset-uids \
     --exclude "/home*" \
     --pkggroup "universe" \
     -y \
     --install=no \
     --pkgrelease "${pkgrel}ubuntu${uburel}" \
     --requires "${pkgdeps}"

