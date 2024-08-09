#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include "common.h"

#define cert0_PARTITION		    cert0_partition
#define cert0_PARTITION_ID	    FIXED_PARTITION_ID(cert0_PARTITION)
#define cert0_PARTITION_DEV	    FIXED_PARTITION_DEVICE(cert0_PARTITION)

#define cert1_PARTITION		    cert1_partition
#define cert1_PARTITION_ID	    FIXED_PARTITION_ID(cert1_PARTITION)
#define cert1_PARTITION_DEV	    FIXED_PARTITION_DEVICE(cert1_PARTITION)

#define SSID_PARTITION          ssid_partition
#define SSID_PARTITION_ID	    FIXED_PARTITION_ID(SSID_PARTITION)
#define SSID_PARTITION_DEV	    FIXED_PARTITION_DEVICE(SSID_PARTITION)

#define PSWD_PARTITION          pswd_partition
#define PSWD_PARTITION_ID	    FIXED_PARTITION_ID(PSWD_PARTITION)
#define PSWD_PARTITION_DEV	    FIXED_PARTITION_DEVICE(PSWD_PARTITION)

#define SSID_SIZE               11
#define PSWD_SIZE               10


LOG_MODULE_REGISTER(flash_update, LOG_LEVEL_DBG);


int flash_load_first_cert(char * cert){
    const struct device *flash_dev;
    off_t fa;

    fa = FIXED_PARTITION_OFFSET(cert0_PARTITION);
    flash_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));

    LOG_INF("fa_offset : 0x%x", fa);
    LOG_INF("fa_dev : %s", flash_dev->name);
    
    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

    if(flash_erase(flash_dev, fa, FLASH_SECTOR_SIZE)<0){
        LOG_ERR("Error during flash erase");
        return 0;
    }

    if(flash_write(flash_dev, fa, cert, CERT_SIZE)<0){
        LOG_ERR("Error during flash write");
        return 0;
    }

    LOG_INF("First certificate loaded successfully");
    
    return 1;
}

int flash_load_new_cert(char * cert){
    const struct device *flash_dev;
    off_t fa;

    fa = FIXED_PARTITION_OFFSET(cert1_PARTITION);
    flash_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));

    LOG_INF("fa_offset : 0x%x", fa);
    LOG_INF("fa_dev : %s", flash_dev->name);
    
    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

    if(flash_erase(flash_dev, fa, FLASH_SECTOR_SIZE)<0){
        LOG_ERR("Error during flash erase");
        return 0;
    }

    if(flash_write(flash_dev, fa, cert, CERT_SIZE)<0){
        LOG_ERR("Error during flash write");
        return 0;
    }

    LOG_INF("New certificate loaded successfully");
    
    return 1;
}

int flash_update_cert(void){
    return 1;
}

char * flash_read_cert(int part_number){
    char *cert;
    const struct device *flash_dev;
    off_t fa;

    if (!part_number) {
        cert = malloc(CERT_SIZE);  
        if (cert == NULL) {
            LOG_ERR("Memory allocation failed.");
            return NULL;
        }
        fa = FIXED_PARTITION_OFFSET(cert0_PARTITION);
        flash_dev = DEVICE_DT_GET(DT_NODELABEL(mx25r6435f));
    }
    else {
        cert = malloc(CERT_SIZE);  
        if (cert == NULL) {
            LOG_ERR("Memory allocation failed.");
            return NULL;
        }
        fa = FIXED_PARTITION_OFFSET(cert1_PARTITION);
        flash_dev = DEVICE_DT_GET(DT_NODELABEL(mx25r6435f));
    }
    

    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

    if(flash_read(flash_dev, fa, cert, CERT_SIZE)<0){
        LOG_ERR("Error during flash read");
        return 0;
    }

    // LOG_HEXDUMP_INF(cert, CERT_SIZE, "CERT BUFFER CONTENT");

    return cert;
}

int swap_certs() {
    char *cert;
    char *new_cert;

    // Read partition 1
    cert = flash_read_cert(0);

    // Read partition 2
    new_cert = flash_read_cert(1);

    // Write partition 1
    flash_load_first_cert(new_cert);

    // Write partition 2
    flash_load_new_cert(cert);

    free(new_cert);
    free(cert);

    LOG_INF("Partitions swapped successfully");

    return 1;
}

int register_wifi_cred(char* ssid, char* pswd){
    const struct device *flash_dev;
    off_t fa_ssid;
    off_t fa_pswd;

    fa_ssid = FIXED_PARTITION_OFFSET(SSID_PARTITION);
    fa_pswd = FIXED_PARTITION_OFFSET(PSWD_PARTITION);

    flash_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));

    LOG_INF("fa_dev : %s", flash_dev->name);
    
    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

    if(flash_erase(flash_dev, fa_ssid, FLASH_SECTOR_SIZE)<0){
        LOG_ERR("Error during flash erase");
        return 0;
    }

    if(flash_erase(flash_dev, fa_pswd, FLASH_SECTOR_SIZE)<0){
        LOG_ERR("Error during flash erase");
        return 0;
    }

    if(flash_write(flash_dev, fa_ssid, ssid, SSID_SIZE)<0){
        LOG_ERR("Error during flash write");
        return 0;
    }

    if(flash_write(flash_dev, fa_pswd, pswd, PSWD_SIZE)<0){
        LOG_ERR("Error during flash write");
        return 0;
    }

    LOG_INF("Wi-Fi cred loaded successfully");
    
    return 1;
}

int wifi_connect_if_cred_registered(void){
    const struct device *flash_dev;
    off_t fa_ssid;
    off_t fa_pswd;
    char* ssid;
    char* pswd;

    printf("\r\nTrying to connect to wifi\r\n");

    ssid = malloc(SSID_SIZE-1);
    if (ssid == NULL) {
        LOG_ERR("Memory allocation failed.");
        return 0;
    }
    pswd = malloc(PSWD_SIZE-2);
    if (pswd == NULL) {
        LOG_ERR("Memory allocation failed.");
        return 0;
    }

    fa_ssid = FIXED_PARTITION_OFFSET(SSID_PARTITION);
    fa_pswd = FIXED_PARTITION_OFFSET(PSWD_PARTITION);

    flash_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));

    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
        free(ssid);
        free(pswd);
		return 0;
	}

    if(flash_read(flash_dev, fa_ssid, ssid, SSID_SIZE)<0){
        LOG_ERR("Error during flash read");
        free(ssid);
        free(pswd);
        return 0;
    }

    if(flash_read(flash_dev, fa_pswd, pswd, PSWD_SIZE)<0){
        LOG_ERR("Error during flash read");
        free(ssid);
        free(pswd);
        return 0;
    }

    printf("\r\nSSID : %s\r\n", ssid);
    printf("\r\nPSWD : %s\r\n", pswd);

    if(strcmp(ssid[0], 0xff)==0){
        LOG_INF("Wifi creds not registered");
        return 0;
    }
    
    if(!connect_WiFi(ssid,pswd)) {
        free(ssid);
        free(pswd);
        return 0;
    }

    free(ssid);
    free(pswd);
    state_wifi=1;
    printf("\r\nSuccessfully connected to Wi-Fi\r\n");
    return 1;

}