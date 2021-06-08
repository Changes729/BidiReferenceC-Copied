#!/bin/bash

SCRIPT_DIR=$(dirname $(readlink -f "$0"))
MAIN_DIR=$SCRIPT_DIR/..
TEST_DIR=$SCRIPT_DIR/../build/bin

$TEST_DIR/bidiref -p $MAIN_DIR/resource/