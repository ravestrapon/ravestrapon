PORT="$(ls /dev/ttyACM* | head -n1)"
BAUD_RATE=9600

stty -F ${PORT} ${BAUD_RATE} raw -clocal -echo icrnl

perl -MTime::HiRes=time -ne '$|=1; printf "%.3f\t%s", time(), $_' < ${PORT}
