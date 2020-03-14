#!/bin/bash
# This will clean the build dir (by removing it!)
this_script_path=`realpath $0`
this_script_dir=`dirname $this_script_path`
build_dir=`realpath "${this_script_dir}/../build"`
rm -rf ${build_dir}
