#!/bin/bash

# Info: This script is used to configure Debian Linux environment in both
#       cli mode with -c flag or user mode with no flags
# Actions:
#   Export environment variables used by LoRaWAN from the .env file (user mode)
#   Install required binaries:
#   gcc-avr avr-libc make libgtest-dev bear (cli mode)
#   gcc-avr avr-libc make libgtest-dev bear gdb-avr avrdude avarice (user mode)

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

if [[ "$cli_mode" = false && ! -f ".env" ]]; then
    # Export environment variables from the .env file
    echo "File '.env' not found"
    read -p "Enter AppKey: " app_key
    read -p "Enter AppEUI: " app_eui
    echo -e "AppKey=${app_key}\nAppEUI=${app_eui}" >>.env
fi

if [[ "$cli_mode" = false ]]; then
    file_content=$(cat .env)
    username=$(whoami)

    # Check if the file is not empty
    if [ ! -z "$file_content" ]; then
        while read -r line; do
            # Simple input validation
            if [[ "$line" =~ ^[a-zA-Z0-9\=]+$ ]]; then
                key="${line%=*}"
                value="${line#*=}"

                if [[ ! -v "$key" ]]; then
                    export_cmd="export $key=\"$value\""
                    export_to_rc_cmd="echo '$export_cmd' >> /home/$username/.bashrc"
                    eval "$export_to_rc_cmd"
                fi
            else
                echo "The env variable \"$line\" contains invalid character(s)"
            fi
        done <<<"$file_content"
    else
        echo "The .env file is empty"
    fi
fi

# List of base packages to check for
packages=("gcc-avr" "avr-libc" "make" "libgtest-dev" "bear")

if [ "$cli_mode" = false ]; then
    # Add more packages in user mode
    packages+=("gdb-avr" "avrdude" "avarice")
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

if [ "$cli_mode" = false ]; then
    exec bash
fi
