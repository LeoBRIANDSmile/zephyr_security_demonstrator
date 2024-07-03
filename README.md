# Source code config
SSID, PWD and IP_ADDRESS should be defined before flashing (wifi.c)

# Building config
To build the project with non-secure communication
=============

    "$ west build -p always -b disco_l475_iot1"


To build the project with secure communication
=============

First, you'll have to generate all the certificates :
    Go to certs repository and execute : $ ./generate_certs.sh
Then, compile the project with the TLS configurations :
    "$ west build -p always -b disco_l475_iot1 -- -DCONF_FILE="prj.conf overlay-tls.conf"" to build with secure communcation





