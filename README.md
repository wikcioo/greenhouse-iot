# Greenhouse IoT Device Firmware

![Build Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/build.yml/badge.svg)
![Test Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/test.yml/badge.svg) 
![Clang Format Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/clang-format-check.yml/badge.svg)
![Deploy Workflow](https://github.com/wikcioo/greenhouse-iot/actions/workflows/deploy.yml/badge.svg)

# Remote SSH environment in Visual Studio Code

1. Install Visual Studio Code [here](https://code.visualstudio.com/download).
2. Install the [Remote - SSH](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-ssh) extension from microsoft.
3. Press the "Remote Explorer" icon, that is found on the sidebar.
4. Add new remote by pressing the add button next to SSH dropdown.
5. Enter `ssh user@domain` in the pop up prompt.
6. Enter the password.
7. Use the command `git clone --recursive https://github.com/wikcioo/greenhouse-iot.git` to clone the repository.
8. Use the command `cd greenhouse-iot` to jump in to the project directory.