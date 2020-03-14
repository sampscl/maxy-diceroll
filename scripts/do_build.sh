#!/bin/bash
# NOTE: This script changes the working directory
this_script_path=`realpath $0`
this_script_dir=`dirname $this_script_path`

build_dir=`realpath "${this_script_dir}/../build"`
mkdir -p ${build_dir} || exit $?
cd ${build_dir} || exit $?

if [ -z ${CMAKE_BUILD_TYPE+x} ]
then
  CMAKE_BUILD_TYPE=Release
fi

echo "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"

if [ -z ${CMAKE_TOOLCHAIN_FILE+x} ]
then
  toolchain_arg=""
else
  toolchain_arg="-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
  echo "Using toolchain ${CMAKE_TOOLCHAIN_FILE}"
fi

echo "Running cmake"
cmake -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ${toolchain_arg} ../ || exit $?

echo "Building project"
make ${TARGET} || exit $?
echo "Built successfully in ${build_dir}"
