# Google_IoT_Device
This repo contains the source for a CMSIS-Pack that enables Arm Cortex-M microcontrollers to connect to the Google IoT platform.

## Instructions
a) Creating the CMSIS-Pack:
   1) run ./gen_pack.sh
   2) install the pack created in ./pack directory (e.g. double click on the pack file).
   3) the component 'IoT Client:Google' is now available in the RTE dialog of your CMSIS-Pack aware tools.

b) Making changes to the CMSIS pack:  
   You can develop and extend this pack further by contributing via GitHub:
   https://github.com/MDK-Packs/Google_IoT_Device

   You can add the ../local/MDK-Packs.Google_IoT_Device.pdsc to the list of local repositories in the PackInstaller.
   This way you avoid to create and install a new pack after modifications.
