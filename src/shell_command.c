#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <ctype.h>
#include "common.h"

static int wifi_connect(const struct shell *sh,
                            size_t argc, char **argv)
{       
        if(argc==3){
            static int ret = 0;
            ret = connect_WiFi(argv[1],argv[2]);
            if(!ret){
                shell_print(sh, "Error when trying to connect");
            }
            else{
                shell_print(sh, "Successfully connected");
            }
        }
        else if(argc<3){
            shell_print(sh, "Too few arguments");          
        }
        else{
            shell_print(sh, "Too much arguments");          
        }

        return 0;
}

static int http_request(const struct shell *sh,
                            size_t argc, char **argv)
{
    if(argc==2){
        static int ret = 0;
        char* get_request[100];

        ret = Socket_Init();
        if(!ret){
            shell_print(sh, "Error during socket connection");
            Socket_Close();
            return 0;
        }
        strcpy(get_request,"GET ");
        shell_print(sh, "%s",get_request);
     
        strcat(get_request,argv[1]);
                shell_print(sh, "%s",get_request);

        strcat(get_request,REQUEST);
                shell_print(sh, "%s",get_request);

        ret = Socket_Send(get_request);
        if(!ret){
            shell_print(sh, "Error while sending request");
            Socket_Close();
            return 0;
        }

        ret = Socket_Receive(dataToRecv);
        if(!ret){
            shell_print(sh, "Error while receiving data");
            Socket_Close();
            return 0;
        }

        Socket_Close();

        return 0;
    }
}

SHELL_STATIC_SUBCMD_SET_CREATE(led_perso,
        SHELL_CMD_ARG(on, NULL, "Led ON", LED_ON,1,0),
        SHELL_CMD_ARG(off, NULL, "Led OFF", LED_OFF,1,0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(led, &led_perso, "Wifi shell commands", NULL);

SHELL_STATIC_SUBCMD_SET_CREATE(device_perso,
        SHELL_CMD_ARG(connect, NULL, "Connect Wi-Fi", wifi_connect,1,2),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(wifi, &device_perso, "Wifi shell commands", NULL);

SHELL_STATIC_SUBCMD_SET_CREATE(http_perso,
        SHELL_CMD_ARG(get, NULL, "Send HTTP request", http_request, 1, 1),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(http, &http_perso, "Socket shell commands", NULL);