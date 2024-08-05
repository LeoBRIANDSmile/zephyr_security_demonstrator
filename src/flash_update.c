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

#define FLASH_SECTOR_SIZE		4096

LOG_MODULE_REGISTER(flash_update, LOG_LEVEL_DBG);


int flash_load_first_cert(char * cert){
    const struct device *flash_dev;
    off_t *fa;

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
    off_t *fa;

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
    off_t *fa;

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

    LOG_HEXDUMP_INF(cert, CERT_SIZE, "CERT BUFFER CONTENT");

    return cert;
}
