#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <ctype.h>
#include "common.h"

static int wifi_connect(const struct shell *sh,
                            size_t argc, char **argv, void *data)
{   
        static int ret = 0;
        ret = connect_WiFi();
        if(!ret){
            shell_print(sh, "Error when trying to connect");
        }
        else    shell_print(sh, "Successfully connected");


        return 0;
}

static int http_request(const struct shell *sh,
                            size_t argc, char **argv, void *data)
{
    static int ret = 0;
    ret = Socket_Init();
    if(!ret){
        shell_print(sh, "Error during socket connection");
        return 0;
    }

    ret = Socket_Send(REQUEST);
    if(!ret){
        shell_print(sh, "Error while sending request");
        return 0;
    }

    ret = Socket_Receive(dataToRecv);
    if(!ret){
        shell_print(sh, "Error while receiving data");
        return 0;
    }

    Socket_Close();

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(led_perso,
        SHELL_CMD_ARG(on, NULL, "Led ON", LED_ON,1,0),
        SHELL_CMD_ARG(off, NULL, "Led OFF", LED_OFF,1,0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(led, &led_perso, "Wifi shell commands", NULL);

SHELL_STATIC_SUBCMD_SET_CREATE(device_perso,
        SHELL_CMD_ARG(connect, NULL, "Connect Wi-Fi", wifi_connect,1,0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(wifi, &device_perso, "Wifi shell commands", NULL);

SHELL_STATIC_SUBCMD_SET_CREATE(http_perso,
        SHELL_CMD_ARG(request, NULL, "Send HTTP request", http_request, 1, 0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(http, &http_perso, "Socket shell commands", NULL);