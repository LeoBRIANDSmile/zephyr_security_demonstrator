# Configuration

Before flashing the project, ensure that you have defined the following parameters in wifi.c:

    SSID: Your WiFi network name
    PWD: Your WiFi network password
    IP_ADDRESS: The IP address of the host

# Building the Project
## Building with Non-Secure Communication

To build the project with non-secure communication, run the following command:

    $ west build -p always -b disco_l475_iot1

## Building with Secure Communication

### Step 1: Generate Certificates
First, generate all the necessary certificates. Navigate to the certs repository and execute:

    $ ./generate_certs.sh

### Step 2: Compile with TLS Configurations

Next, compile the project with TLS configurations by running:

    $ west build -p always -b disco_l475_iot1 -- -DCONF_FILE="prj.conf overlay-tls.conf

### Step 3: Launch HTTPS Server

Then, start the HTTPS server:

    $ sudo python3 https_server/https_server.py

### Step 4: Flash the Project

Finally, flash the project to your device:

    $ west flash





