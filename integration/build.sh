#!/usr/bin/env bash

HELP=$(cat <<-END
Usage: ./build.sh [option]

Build project.

options:
  -h/help    Show the help of the script.
.
END
)

if [ "$1" = "-h" ] || [ "$1" = "help" ]; then
  echo "$HELP"
else

  BUILD_TYPE=$1
  BUILD_TYPE="${BUILD_TYPE:-Release}";

  # Create build folder
#  rm -rf build
  mkdir -p build
  cd build

  # Build project
#  cmake ..
  cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

  # Generate executable
  make

fi
