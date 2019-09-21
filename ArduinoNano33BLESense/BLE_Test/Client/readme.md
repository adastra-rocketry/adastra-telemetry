# Usage

This tool search for BLE devices named "AdAstra Telemetry" and connects to them. You then have the possibility to download and clear the saved data points. 

# Installation

## Prerequirements for bluetooth-hci-socket

1. install node-gyp pre-requirements:

  npm install --global --production windows-build-tools

2. follow the installation guide from https://github.com/noble/node-bluetooth-hci-socket#Windows and replace the BLE driver using Zadig

## run npm install

  It might be neccesary to set the PYTHON env variable when running npm install (point it to your Python 2.7 exe - should get installed when installing the ) 
