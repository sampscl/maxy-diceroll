#!/bin/bash
# This will build the all target for the native platform
this_script_path=`realpath $0`
this_script_dir=`dirname $this_script_path`

if [ -z ${TARGET+x} ]
then
  TARGET=maxy_diceroll
fi

(TARGET=${TARGET} ${this_script_dir}/do_build.sh)
