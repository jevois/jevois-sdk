#!/bin/bash
# USAGE: rebuild-os.sh [-y]
# If -y is specified, assume all default answers are ok
#
# This file is intended to run in the JeVois-A33 development docker

set -e

ver="1.21.0"
gitbranch="master" # "master" for latest, or "v${ver}" for a given version
pkgrel="1"
uburel=`lsb_release -rs`
arch=`dpkg --print-architecture`
pkgver="${ver}-${pkgrel}ubuntu${uburel}"

gcc47="gcc-linaro-arm-linux-gnueabihf-4.7-2013.04-20130415_linux"

if [ "x$1" = "x-y" ]; then usedef=1; else usedef=0; fi
function question { if [ $usedef -eq 1 ]; then REPLY="y"; else read -p "${1}? [Y/n] "; fi }

basedir=`pwd`

# needed by buildroot on ubu >= 19.04:
export FORCE_UNSAFE_CONFIGURE=1

# While jevois-pro moved to gcc-14, here we stick with gcc-10 and c++-20/2a as we have too much legacy code...
depends="gcc-10, g++-10, gfortran-10, gcc-10-cross-base"

compilers=${depends//,/}

# Those should already be installed in the jvsdk docker. They are listed here so we can also have them as dependencies
# in our deb packages:
packages=( build-essential cmake libboost-all-dev autoconf libgtk2.0-dev libjpeg-dev libpng-dev libtiff5-dev
  libxine2-dev libv4l-dev libtbb-dev libfaac-dev python3-numpy python3-pip libgtk-3-dev libopenblas-openmp-dev
  libturbojpeg checkinstall protobuf-compiler libprotobuf-dev libprotoc-dev libtesseract-dev tesseract-ocr-all
  libleptonica-dev git build-essential subversion mercurial doxygen-gui graphviz libjpeg-turbo8-dev autoconf
  libeigen3-dev screen lib32stdc++6 gawk libgtk2.0-dev pkg-config libdc1394-dev libjpeg-dev libpng-dev libtiff5-dev
  libavcodec-dev libavformat-dev libswscale-dev libfaac-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev
  libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils unzip python3-dev libgtk-3-dev libturbojpeg
  libgles2-mesa-dev ncftp emacs libturbojpeg-dev u-boot-tools dpkg-dev flex curl wput libdmtx-dev python-is-python3
  libhtml-parser-perl bzip2 lsb-release liblapacke-dev ${compilers} lib32z1 coreutils )

####################################################################################################
question "Nuke old debs"
if [ "X$REPLY" != "Xn" ]; then
    sudo rm -rf debs
    mkdir debs
fi

####################################################################################################
question "Install build packages"
if [ "X$REPLY" != "Xn" ]; then

    # update to latest:
    sudo apt update
    sudo apt -y upgrade
    sudo apt -y autoremove

    # Install packages unless they are already installed:
    set +e
    failed=""
    for pack in "${packages[@]}"; do
        if `dpkg --list | grep -q $pack`; then
            echo "$pack already installed"
        else
            sudo apt -y install $pack
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
    set -e
fi

# Create the dependencies from the packages:
printf -v pkgdeps '%s,' "${packages[@]}"
pkgdeps="${pkgdeps%,}"

####################################################################################################
question "Nuke /jevois, /var/lib/jevois-*"
if [ "X$REPLY" != "Xn" ]; then
    sudo rm -rf /jevois /var/lib/jevois-*
fi
    
####################################################################################################
question "Rebuild opencv deb"
if [ "X$REPLY" != "Xn" ]; then
    ./rebuild-host-opencv.sh
    mv opencv-*/opencv-*/build/jevois-opencv*.deb debs/
    sudo rm -rf opencv-*
fi

# Fix compilation error with libjpeg-turbo8
if [ ! -f /usr/lib/x86_64-linux-gnu/libturbojpeg.so ]; then
    sudo ln -fs /usr/lib/x86_64-linux-gnu/libturbojpeg.so.0.?.0 /usr/lib/x86_64-linux-gnu/libturbojpeg.so
fi

####################################################################################################
question "Update JeVois software to latest version"
if [ "X$REPLY" != "Xn" ]; then
    for dis in jevois jevoisbase samplemodule samplepythonmodule jevois-tutorials jevois-inventor jevoisextra; do
	    echo "Updating JeVois source code: ${dis} ..."
	    if [ ! -d ${dis} ]; then
	        git clone https://github.com/jevois/${dis}.git -b ${gitbranch}
	    else
	        cd ${dis}
	        git pull
	        cd ..
	    fi
    done
fi

####################################################################################################
question "Rebuild jevois contribs"
if [ "X$REPLY" != "Xn" ]; then
    # Need bazel for jevois contrib tensorflow:
    if [ ! -x /usr/bin/bazel-3.7.2 ]; then
        wget http://jevois.org/data/bazel_3.7.2-linux-x86_64.deb
        sudo dpkg -i bazel_3.7.2-linux-x86_64.deb
        /bin/rm -f bazel_3.7.2-linux-x86_64.deb
    fi
    
    # Re-install all the jevois contrib libraries:
    cd jevois/Contrib
    ./reinstall.sh -y
    cd ../..
    
    # Re-install all the jevoisbase contrib libraries:
    cd jevoisbase/Contrib
    ./reinstall.sh -y
    cd ../..

    # Re-install all the jevoisextra contrib libraries:
    cd jevoisextra/Contrib
    ./reinstall.sh -y
    cd ../..
fi

####################################################################################################
question "Rebuild jevois-sdk-dev deb"
if [ "X$REPLY" != "Xn" ]; then
    # If we already have the kernel cross-compiler, preserve it:
    if [ -x /usr/share/jevois-sdk/${gcc47}/bin/arm-linux-gnueabihf-gcc ]; then
        sudo /bin/mv /usr/share/jevois-sdk/${gcc47} /usr/share/
    fi
        
    # Nuke and re-download the repo:
    sudo /bin/rm -rf /usr/share/jevois-sdk
    cd /usr/share
    sudo git clone https://github.com/jevois/jevois-sdk.git -b ${gitbranch}

    # Get old cross-compiler for the kernel if not already here:
    if [ ! -x /usr/share/jevois-sdk/${gcc47}/bin/arm-linux-gnueabihf-gcc ]; then
        # Did we save it?
        if [ -x /usr/share/${gcc47}/bin/arm-linux-gnueabihf-gcc ]; then
            sudo mv /usr/share/${gcc47} /usr/share/jevois-sdk
        else
            sudo wget http://jevois.org/pkg/${gcc47}.tar.bz2
            sudo tar jxf ${gcc47}.tar.bz2 -C /usr/share/jevois-sdk
            sudo /bin/rm -f ${gcc47}.tar.bz2
        fi
    fi
        
    cd jevois-sdk
    sudo /bin/rm -rf out
    
    set +e # first run on buildroot may give errors...

    # Configure buildroot:
    cd /usr/share/jevois-sdk/buildroot
    sudo make distclean
    sudo /bin/cp configs/sun8i_defconfig .config
    cd ..
    
    # Configure kernel:
    cd linux-3.4
    sudo make distclean
    sudo /bin/cp config-jevois .config
    cd ..
    
    # configure sunxi build system: choose sun8iw5p1, linux, kernel 3.4, jevois-a33
    #echo "Select the following when asked: 0 for sun8iw5p1, 2 for linux, 0 for kernel-3.4, 1 for jevois-a33"
    sudo cat <<EOF | sudo ./build.sh config
0            
2
0
1
EOF

    # build it
    cd /usr/share/jevois-sdk

    sudo ./build.sh # first run may give some error
    set -e
    
    sudo ./jevois-build.sh
    
    depends="build-essential, ${depends}, jevois-sdk (>= ${ver}), u-boot-tools, lib32stdc++6, lib32z1, \
pkg-config, libdmtx-dev"

    # for now, we only need opencv (for TBB includes), cross-compiler, and linux-headers from build/, and let's take
    # host/ and target/ -- everything else can go:
    brdir="out/sun8iw5p1/linux/common/buildroot"
    debdir="deb/usr/share/jevois-sdk/${brdir}"
    sudo /bin/rm -rf deb
    sudo mkdir -p ${debdir}
    sudo /bin/cp -ar ${brdir}/host  ${debdir}/
    sudo /bin/cp -ar ${brdir}/target  ${debdir}/
    sudo mkdir ${debdir}/build
    sudo /bin/cp -ar ${brdir}/build/opencv3-* ${debdir}/build/
    sudo /bin/cp -ar ${brdir}/build/linux-headers-* ${debdir}/build/
    sudo /bin/cp -ar ${gcc47} deb/usr/share/jevois-sdk/

    sudo mkdir deb/DEBIAN

    cat <<EOF | sudo tee deb/DEBIAN/control
Package: jevois-sdk-dev
Architecture: ${arch}
Maintainer: Laurent Itti
Priority: optional
Section: universe
Version: ${pkgver}
Depends: ${depends}
Description: JeVois Smart Machine Vision, Software Development Kit development system
 Development utilities and libraries needed to cross-compile JeVois smart machine vision software
 .
 Provides libraries compiled for the ARM processor inside JeVois, embedded Linux kernel, bootloader,
 and utilities to allow building machine vision modules for the JeVois-A33 hardware platform.
EOF

    echo "==== control file is:"
    cat deb/DEBIAN/control
    echo "==== control file end"
    
    sudo dpkg-deb --build deb jevois-sdk-dev_${pkgver}_${arch}.deb
    sudo mv *.deb "${basedir}/debs/"
    sudo rm -rf deb
    cd "${basedir}"
fi


####################################################################################################
question "Rebuild jevois and jevoisbase debs"
if [ "X$REPLY" != "Xn" ]; then

    # Our cmake will check for jevois-sdk version:
    sudo mkdir -p /var/lib/jevois-build
    sudo cp /usr/share/jevois-sdk/jevois-sdk-version.txt /var/lib/jevois-build/

    cd "${basedir}/jevois"
    sudo rm -rf hbuild pbuild phbuild ppbuild ppdbuild
    ./rebuild-host.sh
    cd hbuild && sudo cpack && cd ..
    ./rebuild-platform.sh
    cd pbuild && sudo cpack && cd ..
    cd ../jevoisbase
    sudo rm -rf hbuild pbuild phbuild ppbuild ppdbuild
    ./rebuild-host.sh
    cd hbuild && sudo cpack && cd ..
    ./rebuild-platform.sh
    cd pbuild && sudo cpack && cd ..
    cd ../jevoisextra
    sudo rm -rf hbuild pbuild phbuild ppbuild ppdbuild
    ./rebuild-all.sh a33
    cd ..

    cd "${basedir}"
    /bin/mv -f jevois/*build/*.deb debs/
    /bin/mv -f jevoisbase/*build/*.deb debs/
    #/bin/mv jevoisextra/*build/*.deb debs/
fi

####################################################################################################
question "Rebuild jevois-sdk deb"
if [ "X$REPLY" != "Xn" ]; then

    cd /usr/share
    cat <<EOF | sudo tee Makefile
# This makefile is here just so that we can use checkinstall to create the jevois-sdk debian package

# keep this in exact sync with jevois-build.sh

.PHONY: all clean

all:
	( cd jevois-sdk && ./jevois-build.sh /tmp/jevois-build )
	@echo "All done"

clean:
	/bin/rm -rf /tmp/jevois-build

install:
	/bin/rm -rf /var/lib/jevois-build/boot /var/lib/jevois-build/jevois-sdk-version.txt
	mkdir -p /var/lib/jevois-build/boot
	install /tmp/jevois-build/boot/u-boot.fex /var/lib/jevois-build/boot
	install /tmp/jevois-build/boot/boot0_sdcard.fex /var/lib/jevois-build/boot
	install /tmp/jevois-build/boot/script.bin /var/lib/jevois-build/boot
	install jevois-sdk/linux-3.4/arch/arm/boot/uImage /var/lib/jevois-build/boot
	install jevois-sdk/out/sun8iw5p1/linux/common/rootfs.ext4 /var/lib/jevois-build/boot
	install jevois-sdk/jevois-build/microsd-readme.txt /var/lib/jevois-build/boot
	install jevois-sdk/jevois-build/uEnv.txt /var/lib/jevois-build/boot
	install jevois-sdk/jevois-build/jevois-flash-card /usr/bin
	install jevois-sdk/jevois-sdk-version.txt /var/lib/jevois-build
EOF

    cat <<EOF | sudo tee description-pak
Software development kit (SDK) for JeVois smart embedded machine vision
EOF

    sudo make all
    sudo make install

    sudo checkinstall \
     --type debian \
     --pkgname jevois-sdk \
     --provides jevois-sdk \
     --pkglicense "GPL v3" \
     --maintainer "Laurent Itti \\<jevois.org@gmail.com\\>" \
     --nodoc \
     --arch "${arch}" \
     --pkgsource="https://github.com/jevois/jevois-sdk/archive/${ver}.tar.gz" \
     --pkgaltsource="https://github.com/jevois/jevois-sdk/tree/${ver}" \
     --pkgversion "${pkgver}" \
     --reset-uids \
     --exclude "/home*" \
     --pkggroup "universe" \
     -y \
     --install=no \
     --pkgrelease "${pkgrel}ubuntu${uburel}" \
     --requires="jevois-host \(= ${pkgver}\),jevois-platform \(= ${pkgver}\),jevoisbase-host \(= ${pkgver}\),jevoisbase-platform \(= ${pkgver}\),${pkgdeps}" \
     make install
    
    sudo /bin/mv jevois-sdk*.deb "${basedir}/debs/"
    sudo /bin/rm Makefile description-pak
fi

####################################################################################################
question "Rebuild microSD disk image"
if [ "X$REPLY" != "Xn" ]; then
    sudo /usr/share/jevois-sdk/jevois-build/jevois-flash-card -y jevois-image-${ver}-8G
    sudo /bin/mv /var/lib/jevois-build/jevois-image-${ver}-8G.zip "${basedir}/debs/"
fi

####################################################################################################
question "Cleanup"
if [ "X$REPLY" != "Xn" ]; then
    cd "${basedir}"
    sudo rm -rf jevois jevoisbase jevoisextra jevois-inventor jevois-tutorials samplemodule samplepythonmodule
fi
