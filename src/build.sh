#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m'


# First use the Arduino compile to generate a .hex file by "verifying" the
# .ino file and specifying a custom build directory
BOARD="MightyCore:avr:16:pinout=standard,clock=16MHz_external"
ACTION="--verify"
ARDUINO="arduino"
INO="ravestrapon.ino"
BUILD_PATH="/tmp/arduino_builds"

echo -e "${CYAN}++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++${NC}"
echo -e "${CYAN}COMPILE: Compiling with the Arduino compiler...${NC}"
JAVA_TOOL_OPTIONS='-Djava.awt.headless=true' "${ARDUINO}" "${ACTION}" "${INO}" --board "${BOARD}" --pref build.path=${BUILD_PATH}
ret=$?
if [ ${ret} -ne 0 ]; then
  echo -e "${RED}ERROR: Arduino returned error code '${ret}'.  Exiting${NC}"
  exit ${ret}
else
  echo -e "${GREEN}SUCCESS: Compilation completed without errors.${NC}"
  echo
fi

# Next, look in that custom build directory and find the .hex file that
# include our program and the arduino bootloader.  Then use avrdude to
# flash that whole thing onto the Atmega16
DEVICE="atmega16"
PROGRAMMER="usbtiny"
AVRDUDE="avrdude -c ${PROGRAMMER} -p ${DEVICE}"

LFUSE="0xFF"
HFUSE="0x89"
VERIFY_FUSES="${AVRDUDE} -U lfuse:v:${LFUSE}:m -U hfuse:v:${HFUSE}:m -qq"
WRITE_FUSES="${AVRDUDE} -U lfuse:w:${LFUSE}:m -U hfuse:w:${HFUSE}:m -qq"
HEX_FILE="${BUILD_PATH}/${INO}.with_bootloader.hex"

# Update Fuses, if need be
echo -e "${CYAN}++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++${NC}"
echo -e "${CYAN}FUSES: Making sure the fuses are L:${LFUSE} and H:${HFUSE}...${NC}"
${VERIFY_FUSES}
if [ $? -ne 0 ]; then
  echo -e "${YELLOW}Incorrect fuse values found, attempting to update them now...${NC}"
  ${WRITE_FUSES}
  echo "Verifying the newly written values..."
  ${VERIFY_FUSES}
  ret=$?
  if [ ${ret} -ne 0 ]; then
    echo -e "${RED}Incorrect fuse values still found! (${ret}) Exiting...${NC}"
    exit ${ret}
  else
    echo -e "${GREEN}Fuse values sucessfully updated.${NC}"
  fi
else
  echo -e "${GREEN}Fuse values appear to be up-to-date, no need to update them.${NC}"
fi
echo

# Perform the actual flashing operation
echo -e "${CYAN}++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++${NC}"
echo -e "${CYAN}FLASHING: Flashing the compiled binary onto the MCU...${NC}"
${AVRDUDE} -U flash:w:${HEX_FILE}:i
ret=$?
if [ ${ret} -ne 0 ]; then
  echo -e "${RED}ERROR: avrdude returned ${ret} while attempting to flash.${NC}"
  exit ${ret}
else
  echo -e "${GREEN}SUCCESS!${NC}"
fi

exit 0
