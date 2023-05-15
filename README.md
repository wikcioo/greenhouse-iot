# Greenhouse IoT Device Firmware

![Build Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/build.yml/badge.svg)
![Test Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/test.yml/badge.svg) 
![Clang Format Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/clang-format-check.yml/badge.svg)
![Deploy Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/deploy.yml/badge.svg)

# Setup development environment on Windows
1. Install VirtualBox [here](https://www.virtualbox.org/wiki/Downloads)
2. Download Debian based Linux iso like [Ubuntu](https://ubuntu.com/download) or [Beryllium](https://www.bunsenlabs.org/installation.html)
3. Create and start a new virtual machine
4. Open a terminal
5. Run `sudo apt update && sudo apt upgrade` to get your system up-to-date
6. Run `sudo apt install linux-headers-$(uname -r) gcc make perl` to install required packages
7. In VirtualBox UI click Devices -> Insert Guest Additions CD image...
8. The previous step should have opened the new folder with mounted CD image; if not navigate to it in your file system
9. Open a terminal in that directory
10. Run `sudo sh ./VBoxLinuxAdditions.run`
11. Reboot
12. Attach USB Devices by clicking Devices -> USB -> device-name
13. Follow [this](#) guide to setup the development environment on Linux

# Remote SSH environment in Visual Studio Code

1. Install Visual Studio Code [here](https://code.visualstudio.com/download).
2. Install the [Remote - SSH](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-ssh) extension from microsoft.
3. Press the "Remote Explorer" icon, that is found on the sidebar.
4. Add new remote by pressing the add button next to SSH dropdown.
5. Enter `ssh user@domain` in the pop up prompt.
6. Enter the password.
7. Use the command `git clone --recursive https://github.com/wikcioo/greenhouse-iot.git` to clone the repository.
8. Use the command `cd greenhouse-iot` to jump in to the project directory.