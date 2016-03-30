#!/bin/zsh

make
echo "\n\nstart rtrd:\n"
./src/rtrd
sleep 1
echo "\n\nstart testrtrd:\n"
./src/testrtrd
sleep 1
echo "\n\n/var/log/messages:\n"
tail -n 10 /var/log/messages
echo "\n\n/var/log/syslog:\n"
tail -n 2 /var/log/syslog
