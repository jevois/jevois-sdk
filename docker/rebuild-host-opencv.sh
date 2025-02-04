#!/bin/bash

# Download, build, and pack opencv for jevois
ma=4
mi=10
pa=0

gccver="10"

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
compilers="gcc-${gccver} g++-${gccver} gfortran-${gccver}"

packages=( build-essential python-is-python3 ${compilers} cmake libboost-all-dev autoconf libeigen3-dev libgtk2.0-dev
    libdc1394-dev libjpeg-dev libpng-dev libtiff5-dev libavcodec-dev libavformat-dev libxine2-dev libgstreamer1.0-dev
    libgstreamer-plugins-base1.0-dev libv4l-dev libtbb-dev libfaac-dev libmp3lame-dev
    libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils unzip
    python${pyver}-dev python3-numpy python3-pip libgtk-3-dev libopenblas-openmp-dev libturbojpeg checkinstall
    protobuf-compiler libprotobuf-dev libtesseract-dev tesseract-ocr-all libleptonica-dev liblapacke-dev
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

# Use our desired gcc version as default:
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${gccver} 100 --slave /usr/bin/g++ g++ /usr/bin/g++-${gccver} --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-${gccver}
sudo update-alternatives --set gcc /usr/bin/gcc-${gccver}

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

# Opencv 4.10 fails to find mpi.h, add eplicit path:
libarch=`uname -p` # x86_64 or aarch64
export CXXFLAGS="-I/usr/include/${libarch}-linux-gnu -I/usr/include/${libarch}-linux-gnu/mpi"

cmake \
    -D CMAKE_C_COMPILER=gcc-${gccver} -D CMAKE_CXX_COMPILER=g++-${gccver} \
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
#!/bin/bash
# fix missing symlinks
for f in /usr/share/jevois-opencv-${ver}/lib/*.so.${ver}; do ln -sf \$f \${f/.${ver}/}; done

# add to ldconfig:
f="/etc/ld.so.conf.d/jevois-opencv.conf"
echo "/usr/share/jevois-opencv-${ver}/lib" > \${f}
p="/usr/share/jevoispro-opencv-${ver}/lib/python${pyver}/dist-packages/cv2/python-${pyver}"
if [ -d "\${p}" ]; then echo "\${p}" >> \${f}; fi
p="/usr/share/jevoispro-opencv-${ver}/lib/python${pyver}/site-packages/cv2/python-${pyver}"
if [ -d "\${p}" ]; then echo "\${p}" >> \${f}; fi

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

