# Polarcube GNU Radio Blocks/Flowgraphs/ClientSW

### Installation

You will need swig and pyephem installed:


```bash
sudo apt-get install swig gnuradio cmake python-pip
pip install pyephem
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

### Interface
In addition to using UFTP there are some python scripts for more efficent lower overhead data transfer.
