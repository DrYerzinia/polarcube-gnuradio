#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/cassini/blocks/gr-spacegrant/lib
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/cassini/blocks/gr-spacegrant/cmake/lib:$PATH
export LD_LIBRARY_PATH=/home/cassini/blocks/gr-spacegrant/cmake/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-spacegrant 
