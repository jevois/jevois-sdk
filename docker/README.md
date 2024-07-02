Docker setup for jevois-sdk
===========================

We provide two docker images:

- jevois-compile-modules: to allow you to create, compile, and package C++ and Python modules for JeVois-A33. Uses the
  corresponding jevois-sdk distribution, which includes all needed cross-compilers and libraries.

- jevois-sdk-build: typically for JeVois staff only, to rebuild the entire OS that runs in the JeVois-A33 camera, and
  generate the deb packages jevois-sdk and jevois-sdk-dev. You may be interested in this image if you want to customize
  the OS that runs in the camera in such a deep way that it cannot be done by just editing files on the provided JeVois
  dstribution microSD images.

Install the latest docker
=========================

For the latest installation instructions, see https://docs.docker.com/engine/install/ubuntu/

For a script that runs these steps for you and more, run ~/jevoispro-sdk/install-dependencies.sh

Nuke any old docker installation:

    for pkg in docker.io docker-doc docker-compose docker-compose-v2 podman-docker containerd runc; do sudo apt-get remove $pkg; done


Add the docker repo to your apt sources:

    # Add Docker's official GPG key:
    sudo apt-get update
    sudo apt-get install ca-certificates curl
    sudo install -m 0755 -d /etc/apt/keyrings
    sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
    sudo chmod a+r /etc/apt/keyrings/docker.asc
    
    # Add the repository to Apt sources:
    echo \
       "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
       $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
       sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
    sudo apt-get update

Install the docker packages:

    sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

Test the installation by running the hello-world image:

    sudo docker run hello-world


Dockerfile-compile-modules
==========================

This is basically the same as installing deb package jevois-sdk-dev on a clean Ubuntu desktop, excapt that all the
dependencies will remain in docker and will not affect your host machine.

You only need to get our script `compile-modules-in-docker.sh` and our docker image, no need to git clone the whole
jevois-sdk repo. The container will run as your regular user, mapping the current directory to /jevois-sdk in the
container. Once in the container, you can create and build modules as if you were doing it on your host after installing
the jevois-sdk-dev deb:

- Create new C++ module: jevois-create-module <VendorName> <ModuleName>
- Create new Python module: jevois-create-python-module <VendorName> <ModuleName>
- Then cd to module directory, edit code, and ./rebuild-platform.sh
- Exit docker and copy the compiled .jvpkg to JEVOIS:/packages/ on your microSD

Example for JeVois version 1.21.0 (you must match it to the version running in your camera):

    docker pull jevois/jevois-compile-modules:1.21.0
    wget https://github.com/jevois/jevois-sdk/blob/master/docker/compile-modules-in-docker.sh
    chmod a+x compile-modules-in-docker.sh
    ./compile-modules-in-docker.sh
    
    # Entering docker container...
    
    jevois-create-module Testing Hello
    cd hello
    
    # Edit code as you wish (you can edit it using an editor running on host too).
    # Edit `postinstall` to match the videomapping you want for your new module.
    
    ./rebuild-platform.sh
    [CTRL-D]
    
    # Exited docker container
    
    # Insert microSD card into a card reader connected to host computer
    cp hello/Testing-Hello.jvpkg /media/${USER}/JEVOIS/packages/
    
    # Unmount microSD and insert into JeVois-A33 camera.

Dockerfile-rebuild-os
=====================

The jevois-sdk docker image will allow you to recompile the entire operating system for JeVois-A33.

This is not required if you just want to develop and cross-compile modules for JeVois-A33: For that, you just need to
install the jevois-sdk-dev Debian package from https://jevois.usc.edu

Get the jevois-a33-rebuild-os image from docker hub
---------------------------------------------------

Example for JeVois version 1.21.0:

    docker pull jevois/jevois-sdk-build:1.21.0
    
    docker run -it -v $(pwd)/debs:/home/jevois/debs jevois/jevois-sdk-build:1.21.0

Once in the container, run:

    sudo ./rebuild-os.sh -y
    
Once complete, exit the container with CTRL-D. The generated deb packages will be in the debs/ directory.

To rebuild the jevois-a33-rebuild-os docker image
=================================================

Usually, only the JeVois core development team would need to do this. Everyone else should pull the pre-built image from
docker hub as explained above.

Replace 1.21.0 below with the JeVois release you want:

    cd jevois-sdk/docker/
    docker build -t jevois/jevois-sdk-build:1.21.0 -f Dockerfile-rebuild-os .

For JeVois core staff only, to push the image to docker hub:

    docker login
    docker push jevois/jevois-sdk-build:1.21.0
    