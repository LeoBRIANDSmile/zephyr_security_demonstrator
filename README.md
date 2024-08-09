# HTTPs Get shell
This project shows how to create a secure IoT object : from the wifi connection until the addition of shell command to automate the task.


# Configuration

Before flashing the project, ensure that you have defined the following parameter in socket.c:

    IP_ADDRESS: The IP address of the host (ip a on the host to determine it)

# Modify the device tree of disco board

&quadspi {
	pinctrl-0 = <&quadspi_clk_pe10 &quadspi_ncs_pe11
		     &quadspi_bk1_io0_pe12 &quadspi_bk1_io1_pe13
		     &quadspi_bk1_io2_pe14 &quadspi_bk1_io3_pe15>;
	pinctrl-names = "default";
	dmas = <&dma1 5 5 0x0000>;
	dma-names = "tx_rx";

	status = "okay";

	mx25r6435f: qspi-nor-flash@90000000 {
		compatible = "st,stm32-qspi-nor";
		reg = <0x90000000 DT_SIZE_M(8)>; /* 64 Mbits */
		qspi-max-frequency = <50000000>;
		status = "okay";

		partitions {
			compatible = "fixed-partitions";
			#address-cells = <1>;
			#size-cells = <1>;

			slot1_partition: partition@0 {
				label = "image-1";
				reg = <0x00000000 DT_SIZE_K(864)>; 	// Secteur 0 a 210
			};
			
			/* Added */
			cert0_partition: partition@d4000 {
				label = "cert-0";
				reg = <0x000d4000 DT_SIZE_K(4)>; 	// Secteur 211
			};
			cert1_partition: partition@d6000{
				label = "cert-1";
				reg = <0x000d6000 DT_SIZE_K(4)>; 	// Secteur 212
			};
			ssid_partition: partition@d8000 {
				label = "ssid";
				reg = <0x000d8000 DT_SIZE_K(4)>; 	// Secteur 213
			};
			pswd_partition: partition@da000{
				label = "pswd";
				reg = <0x000da000 DT_SIZE_K(4)>; 	// Secteur 214
			};
			/* End added */

			storage_partition: partition@dc000 {
				label = "storage";
				reg = <0x000dc000 DT_SIZE_M(7)>;
			};
		};
	};
};

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




