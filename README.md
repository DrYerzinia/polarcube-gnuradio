# Polarcube GNU Radio Blocks/Flowgraphs/ClientSW

### Installation

You will need swig and pyephem installed:


```bash
sudo apt-get install swig gnuradio cmake python-pip
pip2 install pyephem
```

All important blocks needed for flowgraphs are in blocks/gr-spacegrant.

To complie cd into that directory and do:
```bash
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

### Flowgraphs
Located in the GRC directory.
* general_test includes flowgraphs for testing various parts of the polarcube flowgraphs with different SDR's etc.
* polarcube includes flowgraphs for communicating with polarcube with different SDRs.

Using the USRP2_WBX_Tun_TapNode:
* gnuradio-companion must be run as sudo for the TUNTAP device to work
* Remeber to setup AX.25 addressing and RX address filtering.
* UFTP requires libssl-dev to compile
* To get both ends to hear eachother some small Manual Frequency Correction may be required.
* The following network configurations depend on your IP.
  * sudo ifconfig tun0 192.168.200.1
    * 192.168.200.1 is the IP your device will have
  * sudo ip route add 192.168.200.0/24 dev tun0
    * 192.168.200.0/24 is the IP block you want to assign for the tunnel device network
  * sudo ip link set dev tun0 mtu 256
    * AX.25 Link layer can't handle packet sizes over 256
  * ./uftp -I tun0 -H 192.168.0.2 -M 192.168.0.2 -r 2 test.txt
    * Obtain from https://sourceforge.net/projects/uftp-multicast/files/latest/download
    * 192.168.0.2 is the IP of the UFTP server
    * test.txt is the name of the file you want to upload
    * -r 2 creates a longer timeout to give margin for RX/TX switching
  * ./uftpd -I tun0
    * You must specify interface on server side or it fails
* You should be able to ping between devices as well as run UFTP without issues using this configuration.

### Interface
In addition to using UFTP there are some python scripts for more efficent lower overhead data transfer.
