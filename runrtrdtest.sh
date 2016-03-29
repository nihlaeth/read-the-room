#!/bin/zsh

make rtrd
make test-rtrd
echo "\n\nstart rtrd:\n"
./bin/rtrd
echo "\n\nstart testrtrd:\n"
./bin/testrtrd
echo "\n\n/var/log/messages:\n"
tail -n 10 /var/log/messages
echo "\n\n/var/log/syslog:\n"
tail -n 2 /var/log/syslog
