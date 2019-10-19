JeVois smart machine vision camera

Welcome!

Please see http://jevois.org/start for instructions on how to get started with JeVois.

The files on this partition of the microSD card are for advanced users only. Please do not modify them unless you know
what you are doing.

Windows users: The JeVois microSD card actually has 3 partitions, as detailed at
http://http://jevois.org/doc/MicroSD.html and here you are only browsing the BOOT partition, which is the least
interesting (but it has to be the first one for JeVois to properly start up). There are many more files in the other two
partitions (LINUX and JEVOIS) but those may not be vidible on Windows computers.

MacOSX users: The JeVois microSD card actually has 3 partitions, as detailed at
http://http://jevois.org/doc/MicroSD.html and here you are only browsing the BOOT partition, which is the least
interesting (but it has to be the first one for JeVois to properly start up). There are many more files in the other two
partitions (LINUX and JEVOIS), you should be able to access the JEVOIS one while the LINUX one would require extra
software on your Mac for EXT4 filesystem support. Most of the interesting files are in the JEVOIS partition.


In the BOOT partition of JeVois, you can:

- edit uEnv.txt to enable verbose boot messages. Those will be sent to the micro-serial oprt of jevois (4-pin
  connector).

- create an empty file BOOT:login, which will be detected by the JeVois camera when it boots up. If that file is
  present, jevois-daemon will not automatically start, and instead you will get a login prompt on the serial
  console. Username is 'root' with no password. After you login, you can start JeVois software maunally as it would
  have started automatically, by running /usr/bin/jevois.sh

- create an empty file BOOT:nousbserial if you wish to not start the serial-over-USB feature for some reason.

- create an empty file BOOT:nousbsd if you wish to not enable the microSD-over-USB feature for some reason.

- create an empty file BOOT:usbsdauto if you wish to automatically export microSD JEVOIS partition over USB as soon as
  JeVois starts.

- create an empty file BOOT:serialtty if you wish to get a login prompt on the 4-pin hardware serial
  port instead of the JeVois console.

- create an empty file BOOT:usbserialtty if you wish to get a login prompt on the serial-over-USB port instead of the
  JeVois console. Has no effect if BOOT:nousbserial is also present.

- to use a differentg sensor, put its name in a file named BOOT:sensor - for example, if BOOT:sensor exists and contains
  a single line "ov7725" then JeVois will attempt to use an Omnivision ov7725 sensor.
  
