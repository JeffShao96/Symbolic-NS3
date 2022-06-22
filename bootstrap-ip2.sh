#!/bin/bash
#
# This file was automatically generated by s2e-env at 2022-06-21 23:38:01.142750
#
# This bootstrap script is used to control the execution of the target program
# in an S2E guest VM.
#
# When you run launch-s2e.sh, the guest VM calls s2eget to fetch and execute
# this bootstrap script. This bootstrap script and the S2E config file
# determine how the target program is analyzed.
#

set -x



mkdir -p guest-tools32
TARGET_TOOLS32_ROOT=guest-tools32




TARGET_TOOLS_ROOT=${TARGET_TOOLS32_ROOT}



# To save the hassle of rebuilding guest images every time you update S2E's guest tools,
# the first thing that we do is get the latest versions of the guest tools.
function update_common_tools {
    local OUR_S2EGET

    OUR_S2EGET=${S2EGET}
    OUR_S2ECMD=${S2ECMD}

    # First, download the common tools


    for TOOL in ${COMMON_TOOLS}; do
        ${OUR_S2EGET} ${TARGET_TOOLS_ROOT}/${TOOL}
        if [ ! -f ${TOOL} ]; then
          ${OUR_S2ECMD} kill 0 "Could not get ${TOOL} from the host. Make sure that guest tools are installed properly."
          exit 1
        fi
        chmod +x ${TOOL}
    done
}

function update_target_tools {
    for TOOL in $(target_tools); do
        ${S2EGET} ${TOOL} ${TOOL}
        chmod +x ${TOOL}
    done
}

function prepare_target {
    # Make sure that the target is executable
    chmod +x "$1"
}





function get_ramdisk_root {
  echo '/tmp/'
}

function copy_file {
  SOURCE="$1"
  DEST="$2"

  cp ${SOURCE} ${DEST}

}

# This prepares the symbolic file inputs.
# This function takes as input a seed file name and makes its content symbolic according to the symranges file.
# It is up to the host to prepare all the required symbolic files. The bootstrap file does not make files
# symbolic on its own.
function download_symbolic_file {
  SYMBOLIC_FILE="$1"
  RAMDISK_ROOT="$(get_ramdisk_root)"

  ${S2EGET} "${SYMBOLIC_FILE}"
  if [ ! -f "${SYMBOLIC_FILE}" ]; then
    ${S2ECMD} kill 1 "Could not fetch symbolic file ${SYMBOLIC_FILE} from host"
  fi

  copy_file "${SYMBOLIC_FILE}" "${RAMDISK_ROOT}"

  SYMRANGES_FILE="${SYMBOLIC_FILE}.symranges"

  ${S2EGET} "${SYMRANGES_FILE}" > /dev/null

  # Make the file symbolic
  if [ -f "${SYMRANGES_FILE}" ]; then
     export S2E_SYMFILE_RANGES="${SYMRANGES_FILE}"
  fi


  # The symbolic file will be split into symbolic variables of up to 4k bytes each.
  ${S2ECMD} symbfile 4096 "${RAMDISK_ROOT}${SYMBOLIC_FILE}" > /dev/null

}

function download_symbolic_files {
  for f in "$@"; do
    download_symbolic_file "${f}"
  done
}



# This function executes the target program given in arguments.
#
# There are two versions of this function:
#    - without seed support
#    - with seed support (-s argument when creating projects with s2e_env)
function execute {
    local TARGET

    TARGET="$1"
    shift

    execute_target "${TARGET}" "$@"
}



###############################################################################
# This section contains target-specific code

function make_seeds_symbolic {
    echo 1
}

# This function executes the target program.
# You can customize it if your program needs special invocation,
# custom symbolic arguments, etc.
function execute_target {
    local TARGET

    TARGET="$1"
    shift


    S2E_SO="${TARGET_TOOLS32_ROOT}/s2e.so"



    "${TARGET}" $* > /dev/null 2> /dev/null

}

# Nothing more to initialize on Linux
function target_init {
    # Start the LinuxMonitor kernel module
    sudo modprobe s2e
}

# Returns Linux-specific tools
function target_tools {

    echo "${TARGET_TOOLS32_ROOT}/s2e.so"

}

S2ECMD=./s2ecmd
S2EGET=./s2eget
S2EPUT=./s2eput
COMMON_TOOLS="s2ecmd s2eget s2eput"

###############################################################################


update_common_tools
update_target_tools



# Don't print crashes in the syslog. This prevents unnecessary forking in the
# kernel
sudo sysctl -w debug.exception-trace=0

# Prevent core dumps from being created. This prevents unnecessary forking in
# the kernel
ulimit -c 0

# Ensure that /tmp is mounted in memory (if you built the image using s2e-env
# then this should already be the case. But better to be safe than sorry!)
if ! mount | grep "/tmp type tmpfs"; then
    sudo mount -t tmpfs -osize=10m tmpfs /tmp
fi

# Need to disable swap, otherwise there will be forced concretization if the
# system swaps out symbolic data to disk.
sudo swapoff -a



target_init

cd ns-3-dev
./waf --run  IPEfficientSymEx


# Download the target file to analyze



download_symbolic_files



# Run the analysis



    TARGET_PATH='./None'




prepare_target "${TARGET_PATH}"





execute "${TARGET_PATH}"
