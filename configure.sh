#!/bin/bash

# Info: This script is used to configure Debian Linux environment in both
#       cli mode with -c flag or user mode with no flags
# Actions:
#   Create env.h header file containing LoRaWAN keys
#   Install required binaries:
#   Cli Mode:
#       gcc-avr avr-libc make libgtest-dev
#
#   User Mode:
#       gcc-avr avr-libc make libgtest-dev
#       gdb-avr avrdude avarice bear minicom clang-format

cli_mode=false

while getopts ":c" opt; do
    case $opt in
    c)
        cli_mode=true
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
        ;;
    esac
done

env_header_file="include/env.h"

if [[ "$cli_mode" = false && ! -f "$env_header_file" ]]; then
    # Export environment variables from the .env file
    echo "File '$env_header_file' not found"
    read -p "Enter AppKey: " app_key
    read -p "Enter AppEUI: " app_eui
    echo -e "#define LORA_appKEY \"${app_key}\"\n#define LORA_appEUI \"${app_eui}\"" >>$env_header_file
fi

# List of base packages to check for
packages=("gcc-avr" "avr-libc" "make" "libgtest-dev")

if [ "$cli_mode" = false ]; then
    # Add more packages in user mode
    packages+=("gdb-avr" "avrdude" "avarice" "bear" "minicom" "clang-format")
fi

missing=()

# Define color codes
red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m' # No Color

function echo_color() {
    echo -e "$1$2${NC}"
}

# Loop through packages
echo "Checking for required packages:"
for pkg in "${packages[@]}"; do
    # Check if package is installed
    if dpkg-query -W -f='${Status}\n' $pkg 2>/dev/null | grep -q "^install ok*"; then
        echo_color "$green" "\"$pkg\" found"
    else
        echo_color "$red" "\"$pkg\" not found"
        missing+=("$pkg")
    fi
done

if [ ${#missing[@]} -ne 0 ]; then
    if [ "$cli_mode" = true ]; then
        eval "sudo apt install ${missing[@]}"
    else
        echo "The following packages are missing: ${missing[@]}"
        read -p "Do you want to install them now? (Y/n) " answer

        if [ "$answer" = "y" ] || [ "$answer" = "Y" ] || [ "$answer" = "" ]; then
            eval "sudo apt install ${missing[@]}"
        else
            echo "Not installing"
        fi
    fi
else
    echo "All required packages are installed"
fi
