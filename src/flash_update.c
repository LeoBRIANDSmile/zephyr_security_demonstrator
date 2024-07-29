#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <zephyr/logging/log.h>

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
    const struct flash_area *fa;
    // off_t flash_area = FIXED_PARTITION_OFFSET(cert0_PARTITION);

    fa = FIXED_PARTITION_OFFSET(cert0_PARTITION);
    flash_dev = DEVICE_DT_GET(DT_NODELABEL(mx25r6435f));

    // if(flash_area_open(cert0_PARTITION_ID, &flash_area)==0){
    //     printf("\r\nError during flash open\r\n");
    //     return 0;
    // }

    LOG_INF("fa_offset : %d", fa->fa_off);


    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

    if(flash_erase(flash_dev, fa->fa_off, FLASH_SECTOR_SIZE)<0){
        LOG_ERR("\r\nError during flash erase\r\n");
        return 0;
    }
    LOG_INF("fa_offset : %d", fa->fa_off);
    if(flash_write(flash_dev, fa->fa_off, cert, sizeof(cert))<0){
        LOG_ERR("\r\nError during flash write\r\n");
        return 0;
    }
    
    return 1;
}

int flash_load_new_cert(char * cert){
    const struct device *flash_dev;
    // const struct flash_area *flash_area;
    off_t flash_area = FIXED_PARTITION_OFFSET(cert0_PARTITION);

    // flash_area = FIXED_PARTITION_OFFSET(cert0_PARTITION);
    flash_dev = DEVICE_DT_GET(DT_NODELABEL(mx25r6435f));

    // if(flash_area_open(cert0_PARTITION_ID, &flash_area)==0){
    //     printf("\r\nError during flash open\r\n");
    //     return 0;
    // }

    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

    if(flash_erase(flash_dev, flash_area, FLASH_SECTOR_SIZE)<0){
        LOG_ERR("\r\nError during flash erase\r\n");
        return 0;
    }
    LOG_INF("fa_offset : %d", flash_area);
    if(flash_write(flash_dev, flash_area, cert, sizeof(cert))<0){
        LOG_ERR("\r\nError during flash write\r\n");
        return 0;
    }
    
    return 1;
}

int flash_update_cert(void){
    return 1;
}

char * flash_read_cert(void){
    char cert[2000];
    const struct device *flash_dev;
    const struct flash_area *flash_area;

    flash_area = FIXED_PARTITION_OFFSET(cert0_PARTITION);
    flash_dev = DEVICE_DT_GET(DT_NODELABEL(mx25r6435f));

    if (!device_is_ready(flash_dev)) {
		LOG_ERR("%s: device not ready.\n", flash_dev->name);
		return 0;
	}

    if(flash_read(flash_dev, flash_area, cert, sizeof(cert))<0){
        LOG_ERR("\r\nError during flash read\r\n");
        return 0;
    }
    return cert;
}
