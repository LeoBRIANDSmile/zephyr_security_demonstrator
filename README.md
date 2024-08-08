# HTTPs Get shell
This project shows how to create a secure IoT object : from the wifi connection until the addition of shell command to automate the task.


# Configuration

Before flashing the project, ensure that you have defined the following parameter in socket.c:

    IP_ADDRESS: The IP address of the host (ip a on the host to determine it)

# Building the Project
## Building with Non-Secure Communication

To build the project with non-secure communication, run the following command:

    $ west build -p always -b disco_l475_iot1

## Building with Secure Communication

### Step 1: Generate Certificates
First, generate all the necessary certificates. Navigate to the certs repository (cd certs) and execute:

    $ ./generate_certs.sh

### Step 2: Compile with TLS Configurations

Next, compile the project with TLS configurations by running:

    $ west build -p always -b disco_l475_iot1 -- -DCONF_FILE="prj.conf overlay-tls.conf

### Step 3: Depends on your needs : 

#### a. Launch HTTPS Server

Then, start the HTTPS server from /disco_wifi_socket:

    $ sudo python3 https_server/https_server.py

#### b. Launch ncat server

Then, start the ncat server from /disco_wifi_socket:

    $ sudo ncat -lvnp 443 --ssl --ssl-key certs/host-key.pem --ssl-cert certs/host-cert.pem -k


### Step 4: Flash the Project

Finally, flash the project to your device:

    $ west flash

### Step 4: Flash the Project

Go to MCUboot project in zephyrproject/bootloader/mcuboot/boot/zephyr

Modify it in order to add binary signature whe building

And then build and flash : 

    $ west build -p always -b disco_l475_iot1

    $ west flash


# Testing the project

On the UART terminal : 
$ wifi connect <YOUR_SSID> <YOUR_PSWD>
$ http get <your_file_path>




