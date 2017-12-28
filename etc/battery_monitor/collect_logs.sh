PORT="$(ls /dev/ttyACM* | head -n1)"
BAUD_RATE=9600

stty -F ${PORT} ${BAUD_RATE} raw -clocal -echo icrnl

cat < ${PORT} |  awk '{ print strftime("%m/%d %H:%M:%S"), $0; fflush(); }'
