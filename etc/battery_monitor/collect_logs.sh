PORT="$(ls /dev/ttyACM* | head -n1)"
BAUD_RATE=9600

stty -F ${PORT} ${BAUD_RATE} raw -clocal -echo icrnl

cat < ${PORT} | perl -MTime::HiRes=time -ne 'printf "%.3f\t%s", time(), $_'
