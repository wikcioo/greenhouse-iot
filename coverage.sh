#!/bin/bash

# Info: This script is used to generate a coverage report

# Define color codes
red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m' # No Color

function echo_color() {
    echo -e "$1$2${NC}"
}

function print_command_status() {
    if [ $? -eq 0 ]; then
        echo_color "$green" "OK" "$NC"
    else
        echo_color "$red" "ERR" "$NC"
        clean_up
        exit 1
    fi
}

function clean_up() {
    echo -n "Cleaning up... "
    make clean > /dev/null 2>&1
    print_command_status
}

cd tests
echo -n "Compiling tests and generating coverage notes... "
make GEN_COV=true > /dev/null 2>&1
print_command_status

echo -n "Running tests and generating coverage data... "
./build/gtest_tests > /dev/null 2>&1
print_command_status

echo -n "Generating coverage report in tests/coverage/... "
mkdir -p "coverage"
gcovr -r .. --html -o coverage/coverage_report.html
print_command_status

clean_up