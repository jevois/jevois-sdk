#!/bin/bash
# USAGE: rebuild-os.sh [-y]
# If -y is specified, assume all default answers are ok
#
# This file is intended to run in the JeVois-A33 development docker

ver="1.21.0"
gitbranch="master" # "master" for latest, or "v${ver}" for a given version
pkgrel="1"
uburel=`lsb_release -rs`
arch=`dpkg --print-architecture`
pkgver="${ver}-${pkgrel}ubuntu${uburel}"

if [ "x$1" = "x-y" ]; then usedef=1; else usedef=0; fi
function question { if [ $usedef -eq 1 ]; then REPLY="y"; else read -p "${1}? [Y/n] "; fi }

basedir=`pwd`

# needed by buildroot on ubu 19.04:
export FORCE_UNSAFE_CONFIGURE=1

# Select gcc version for host builds:
if [ "${uburel}" = "15.10" ]; then
	depends="gcc-5, g++-5, gfortran-5";
    crossdep="gcc-7-cross-base"
    qt4lib="libqt4-dev libfaac-dev"
elif [ "${uburel}" = "16.04" ]; then
	depends="gcc-6, g++-6, gfortran-6";
    crossdep=""
    qt4lib="libqt4-dev libfaac-dev"
elif [ "${uburel}" = "17.10" ]; then
	depends="gcc-7, g++-7, gfortran-7";
    qt4lib="libqt4-dev libfaac-dev"
elif [ "${uburel}" = "18.04" -o "${uburel}" = "19.04" ]; then
	depends="gcc-8, g++-8, gfortran-8";
    crossdep="gcc-7-cross-base"
    qt4lib="libqt4-dev libfaac-dev"
elif [ "${uburel}" = "20.04" ]; then
	depends="gcc-10, g++-10, gfortran-10";
    crossdep="gcc-10-cross-base"
    qt4lib=""
elif [ "${uburel}" = "24.04" ]; then
    # While jevois-pro moved to gcc-14, here we stick with gcc-10 and c++-20/2a as we have too much legacy code...
	depends="gcc-10, g++-10, gfortran-10, python-is-python3";
    crossdep="gcc-10-cross-base"
    qt4lib=""
else
    echo "Unsupported Ubuntu version -- ABORT"
    exit 1
fi

compilers=${depends//,/}

# Those should already be installed in th ejvsdk docker. They are listed here so we can also have them as dependencies
# in our deb packages:
packages=( build-essential cmake libboost-all-dev autoconf libgtk2.0-dev libjpeg-dev libpng-dev libtiff5-dev
  libxine2-dev libv4l-dev libtbb-dev ${qt4lib} libfaac-dev python3-numpy python3-pip libgtk-3-dev libatlas-base-dev
  libturbojpeg checkinstall protobuf-compiler libprotobuf-dev libprotoc-dev libtesseract-dev tesseract-ocr-all
  libleptonica-dev git build-essential subversion mercurial doxygen-gui graphviz libjpeg-turbo8-dev autoconf
  libeigen3-dev screen lib32stdc++6 gawk libgtk2.0-dev pkg-config libdc1394-dev libjpeg-dev libpng-dev libtiff5-dev
  libavcodec-dev libavformat-dev libswscale-dev libfaac-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev
  libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils unzip python3-dev libgtk-3-dev libturbojpeg
  libgles2-mesa-dev ncftp emacs libturbojpeg-dev u-boot-tools dpkg-dev flex curl wput libdmtx-dev python-is-python3
  libhtml-parser-perl bzip2 lsb-release )

####################################################################################################
question "Install build packages"
if [ "X$REPLY" != "Xn" ]; then

    # update to latest:
    sudo apt update
    sudo apt -y upgrade
    sudo apt -y autoremove

    # get cross-compiler installed if not already here:
    if [ ! -x /usr/bin/arm-linux-gnueabi-gcc ]; then

        # We need gcc-4.7-arm-linux-gnueabi to compile our kernel. If we have a newer version, remove it:
        sudo apt -y remove libc6-armel-cross linux-libc-dev-armel-cross binutils-arm-linux-gnueabi \
             cpp-5-arm-linux-gnueabi gcc-5-arm-linux-gnueabi-base gcc-5-cross-base gcc-6-cross-base gcc-7-cross-base \
             gcc-8-cross-base gcc-9-cross-base gcc-10-cross-base gcc-11-cross-base gcc-12-cross-base gcc-13-cross-base \
             gcc-14-cross-base
        sudo apt -y autoremove

        wget http://jevois.org/data/gcc4.7-xenial.tbz
        tar jxvf gcc4.7-xenial.tbz
        dpkg -i gcc4.7-xenial/*.deb
        /bin/rm -rf gcc4.7-xenial gcc4.7-xenial.tbz
        sudo ln -fs /usr/bin/arm-linux-gnueabi-gcc-4.7 /usr/bin/arm-linux-gnueabi-gcc
    fi
    
    # get compilers
    sudo apt -y install $compilers
    
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
    mv opencv-*/opencv-*/build/jevois-opencv*.deb .
    #sudo rm -rf opencv-*
fi

# Fix compilation error with libjpeg-turbo8
if [ ! -f /usr/lib/x86_64-linux-gnu/libturbojpeg.so ]; then
    sudo ln -fs /usr/lib/x86_64-linux-gnu/libturbojpeg.so.0.?.0 /usr/lib/x86_64-linux-gnu/libturbojpeg.so
fi

####################################################################################################
question "Update JeVois software to latest version"
if [ "X$REPLY" != "Xn" ]; then
    if [ ! -x /usr/bin/git ]; then sudo apt -y install git; fi
    
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
    sudo dpkg -i bazel_3.7.2-linux-x86_64.deb
    
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
    question "Nuke any old jevois-sdk install"
    if [ "X$REPLY" != "Xn" ]; then
	    sudo /bin/rm -rf /usr/share/jevois-sdk
	    cd /usr/share
	    sudo git clone https://github.com/jevois/jevois-sdk.git -b ${gitbranch}

	    cd jevois-sdk
	    sudo rm -rf out
        
	    # Configure buildroot:
	    cd buildroot
	    sudo make distclean
	    sudo cp configs/sun8i_defconfig .config
	    cd ..
        
	    # Configure kernel:
	    cd linux-3.4
	    sudo make distclean
	    sudo cp config-jevois .config
	    cd ..
        
        # We need python (and not just python3) for that build...
        sudo apt -y install python-is-python3
        
	    # configure sunxi build system: choose sun8iw5p1, linux, kernel 3.4, jevois-a33
	    #echo "Select the following when asked: 0 for sun8iw5p1, 2 for linux, 0 for kernel-3.4, 1 for jevois-a33"
	    sudo cat <<EOF | sudo ./build.sh config
0            
2
0
1
EOF
    fi

    # build it
    cd /usr/share/jevois-sdk
    sudo ./build.sh # the first run may give some error
    sudo ./build.sh
    sudo ./build.sh pack

    sudo ./jevois-build.sh
    
    # Build debian package
    if [ -x /usr/bin/arm-linux-gnueabi-gcc-4.7 ]; then
	    gccdep="gcc-4.7-arm-linux-gnueabi"
    elif [ -x /usr/bin/arm-linux-gnueabi-gcc-5 ]; then
	    gccdep="gcc-5-arm-linux-gnueabi"
    else
	    echo "ooops where is gcc-4.7-arm-linux-gnueabi ?"
	    exit 8
    fi
    
    depends="build-essential, ${depends}, jevois-sdk (>= ${ver}), libc6-armel-cross, libgcc1-armel-cross, libgomp1-armel-cross, binutils-arm-linux-gnueabi, linux-libc-dev-armel-cross, libc6-dev-armel-cross, ${gccdep}, gawk, libgtk2.0-dev, u-boot-tools, lib32stdc++6, pkg-config, libdmtx-dev"

    if [ "X${crossdep}" != "X" ]; then depends="${depends}, ${crossdep}"; fi

    # for now, we only need opencv (for TBB includes) and linux-headers from build/, and let's take host/ and target/ --
    # everything else can go:
    brdir="out/sun8iw5p1/linux/common/buildroot"
    debdir="deb/usr/share/jevois-sdk/${brdir}"
    sudo rm -rf deb
    sudo mkdir -p ${debdir}
    sudo cp -ar ${brdir}/host  ${debdir}/
    sudo cp -ar ${brdir}/target  ${debdir}/
    sudo mkdir ${debdir}/build
    sudo cp -ar ${brdir}/build/opencv3-*  ${debdir}/build/
    sudo cp -ar ${brdir}/build/linux-headers-*  ${debdir}/build/

    sudo mkdir deb/DEBIAN

    cat >/tmp/control <<EOF
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
 Provides cross-compilers, libraries compiled for the ARM processor inside JeVois, embedded Linux kernel
 bootloaders, and utilities to allow building machine vision modules for the JeVois hardware platform.
EOF

    sudo mv /tmp/control deb/DEBIAN/
    echo "==== control file is:"
    cat deb/DEBIAN/control
    echo "==== control file end"
    
    cat > /tmp/postinst <<EOF
#!/bin/sh

if [ ! -f /usr/bin/arm-linux-gnueabi-gcc -a ! -h /usr/bin/arm-linux-gnueabi-gcc ]; then
  if [ -x /usr/bin/arm-linux-gnueabi-gcc-4.7 ]; then
    ln -fs /usr/bin/arm-linux-gnueabi-gcc-4.7 /usr/bin/arm-linux-gnueabi-gcc
  elif [ -x /usr/bin/arm-linux-gnueabi-gcc-5 ]; then
    ln -fs /usr/bin/arm-linux-gnueabi-gcc-5 /usr/bin/arm-linux-gnueabi-gcc
  else
    echo "WARNING: make sure arm-linux-gnueabi-gcc with version 5 or less is installed"
  fi
fi
EOF
    
    sudo mv /tmp/postinst deb/DEBIAN/
    sudo chmod a+x deb/DEBIAN/postinst

    sudo dpkg-deb --build deb jevois-sdk-dev_${pkgver}_${arch}.deb
    sudo mv *.deb "${basedir}/"
    sudo rm -rf deb
    cd "${basedir}"
fi


####################################################################################################
question "Rebuild jevois and jevoisbase debs"
if [ "X$REPLY" != "Xn" ]; then
	cd jevois \
	&& sudo rm -rf hbuild pbuild phbuild ppbuild ppdbuild \
	&& ./rebuild-host.sh \
	&& cd hbuild && sudo cpack && cd .. \
	&& ./rebuild-platform.sh \
	&& cd pbuild && sudo cpack && cd .. \
	&& cd ../jevoisbase \
	&& sudo rm -rf hbuild pbuild phbuild ppbuild ppdbuild \
	&& ./rebuild-host.sh \
	&& cd hbuild && sudo cpack && cd .. \
	&& ./rebuild-platform.sh --staging \
	&& cd pbuild && sudo cpack && cd .. \
	&& cd ../jevoisextra \
	&& sudo rm -rf hbuild pbuild phbuild ppbuild ppdbuild \
	&& ./rebuild-all.sh \
	&& cd ..
    cd "${basedir}"
    mv jevois/*build/*.deb .
    mv jevoisbase/*build/*.deb .
    #mv jevoisextra/*build/*.deb .
fi

####################################################################################################
question "Rebuild jevois-sdk deb"
if [ "X$REPLY" != "Xn" ]; then
    cd "${basedir}"
    sudo cp Makefile description-pak /usr/share/
    cd /usr/share

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
    
    mv jevois-sdk*.deb "${basedir}/"
    sudo /bin/rm Makefile description-pak
fi

