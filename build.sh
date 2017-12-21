#!/bin/sh
set -e

SCRIPT_FILE=`readlink -f "$0"`
SCRIPT_DIR=`dirname "$SCRIPT_FILE"`
cd "$SCRIPT_DIR"

case `uname -s` in
CYGWIN*)
  DEFAULT_VS="Visual Studio 15 2017";
  if [ `uname -m` = x86_64 ]; then DEFAULT_VS="$DEFAULT_VS Win64"; fi
  : "${GENERATOR:=$DEFAULT_VS}"
  ;;
*)
  DEFAULT_MAKE="Unix Makefiles"
  if (ninja --version) >/dev/null 2>&1; then DEFAULT_MAKE=Ninja; fi
  : "${GENERATOR:=$DEFAULT_MAKE}"
  ;;
esac

: "${CONFIGURATION:=RelWithDebInfo}"

mkdir bld
cd bld
cmake -DCMAKE_BUILD_TYPE="$CONFIGURATION" -G "$GENERATOR" ..
cmake --build . --config "$CONFIGURATION"
