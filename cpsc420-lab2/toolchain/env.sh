#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export CS505_HOME=$SCRIPT_DIR

export LD_LIBRARY_PATH=/usr/lib64

export PATH=$CS505_HOME/local/bin:$CS505_HOME/local/encap/maven-sys-xcc/bin:$PATH
export LD_LIBRARY_PATH=$CS505_HOME/local/lib:$CS505_HOME/local/encap/maven-sys-xcc/lib:$LD_LIBRARY_PATH
export MANPATH=$CS505_HOME/local/man:$MANPATH
export C_INCLUDE_PATH=$CS505_HOME/local/include
export CPLUS_INCLUDE_PATH=$CS505_HOME/local/include

######################
# Synopsys
######################
#source /usr/licensed/synopsys/profile.20151209
function urg() { command urg -full64 "$@"; }; export -f urg
function vcs() { command vcs -full64 "$@"; }; export -f vcs
function dve() { command dve -full64 "$@"; }; export -f dve
#export SYN_HOME="/usr/licensed/synopsys/syn_I-2013.12-SP4"
#export LM_LICENSE_FILE=2100@raas03:36371@raas03:5280@raas03:1717@raas03

# workaround for g++ path
export PATH=/usr/bin/:$PATH


source $CS505_HOME/local/encap_source.sh
