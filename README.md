# Google_IoT_Device
This repo contains the source for a CMSIS-Pack that enables Arm Cortex-M microcontrollers to connect to the [Google IoT platform](https://cloud.google.com/solutions/iot/).

## Prerequisites
Working with this repository requires the following applications and packs to be installed on your PC:
- bash compatible shell (under Windows, use for example [git bash](https://gitforwindows.org/))
- ZIP archive creation utility (e.g. [7-Zip](https://www.7-zip.org/))

## Instructions
1. Open a bash compatible shell
2. Clone the repository: `git clone https://github.com/MDK-Packs/Google_IoT_Device.git`
3. Run `./gen_pack.sh` to create a CMSIS-Pack file
5. You can now add the `./MDK-Packs.Google_IoT_Device.pdsc` to the list of local repositories in Pack Installer. This enables direct access to its content without the need to re-build and re-install the pack after modifications.
4. Alternatively, install the pack created in the root directory (e.g. double-click on the pack file)
5. The component **IoT Client:Google** is now available in the RTE dialog of your CMSIS-Pack aware tools.

### Making changes to the CMSIS-Pack
Contributions are welcome. Please raise a pull-request via GitHub: https://github.com/MDK-Packs/Google_IoT_Device.

## Directory Structure

| Directory          | Content                                             |
|:-------------------|:----------------------------------------------------|
| config             | Mbed TLS configuration file              |
| doc                | Google IoT Device pack documentation     |
| include            | Header file with the API                 |
| source             | Source files                             |
| template           | Project template files                   |

## Other related GitHub repositories

| Repository                  | Description                                               |
|:--------------------------- |:--------------------------------------------------------- |
| [CMSIS](https://github.com/ARM-software/cmsis_5)                 | CMSIS                                                                             |
| [CMSIS-FreeRTOS](https://github.com/arm-software/CMSIS-FreeRTOS) | CMSIS-RTOS adoption of FreeRTOS                                                   |
| [CMSIS-Driver](https://github.com/arm-software/CMSIS-Driver)     | Generic MCU driver implementations and templates for Ethernet MAC/PHY and Flash   |
| [MDK-Packs](https://github.com/mdk-packs)                        | IoT cloud connectors as trail implementations for MDK (help us to make it generic)|

## License
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
