#!/bin/sh
#
# Use this script to run your program LOCALLY.
#
# Note: Changing this script WILL NOT affect how CodeCrafters runs your program.
#
# Learn more: https://codecrafters.io/program-interface

set -e # Exit early if any commands fail

# Copied from .codecrafters/compile.sh
#
# - Edit this to change how your program compiles locally
# - Edit .codecrafters/compile.sh to change how your program compiles remotely
(
  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
  CMAKE_ARGS=""
  if [ -n "$VCPKG_ROOT" ]; then
    CMAKE_ARGS="-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
  fi
  if command -v mingw32-make >/dev/null 2>&1; then
    cmake -B build -S . -G "MinGW Makefiles" $CMAKE_ARGS
  else
    cmake -B build -S . $CMAKE_ARGS
  fi
  cmake --build ./build
)
 
# Copied from .codecrafters/run.sh
#
# - Edit this to change how your program runs locally
# - Edit .codecrafters/run.sh to change how your program runs remotely
if [ -f "./build/exe.exe" ]; then
  exec ./build/exe.exe "$@"
else
  exec ./build/exe "$@"
fi
