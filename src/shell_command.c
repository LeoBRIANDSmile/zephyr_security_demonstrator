#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <ctype.h>
#include "common.h"
#include <zephyr/net/tls_credentials.h>
#include "ca_certificate.h"

int state_wifi=0;

// Led commands

SHELL_STATIC_SUBCMD_SET_CREATE(led_perso,
        SHELL_CMD_ARG(on, NULL, "Led ON", LED_ON,1,0),
        SHELL_CMD_ARG(off, NULL, "Led OFF", LED_OFF,1,0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(led, &led_perso, "Wifi shell commands", NULL);


// Wifi commands

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
                state_wifi=1;
                return 1;
            }
        }
        else if(argc<3){
            shell_print(sh, "Too few arguments");
            return 0;         
        }
        else{
            shell_print(sh, "Too much arguments");          
            return 0;         
        }
}

SHELL_STATIC_SUBCMD_SET_CREATE(wifi_perso,
        SHELL_CMD_ARG(connect, NULL, "Connect to Wi-Fi\n This command requires 2 arguments : wifi connect <SSID> <PSWD>", wifi_connect,1,2),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(wifi, &wifi_perso, "Wifi shell commands", NULL);


// HTTP commands

static int http_request(const struct shell *sh,
                            size_t argc, char **argv)
{
    if(state_wifi){
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
            strcat(get_request,argv[1]);
            strcat(get_request,REQUEST);

            ret = Socket_Send(get_request);
            if(!ret){
                shell_print(sh, "Error while sending request");
                Socket_Close();
                return 0;
            }

            ret = Socket_Receive(dataToRecv);
            if(!ret){
                // shell_print(sh, "Error while receiving data");
                Socket_Close();
                return 0;
            }

            Socket_Close();

            return 1;
        }
        else{
            shell_print(sh, "Please specify the relative path of the file you want to get.");
        }
    }
    else{
        shell_print(sh, "Device not connected to wifi, please connect before sending http request.");
    }
}

SHELL_STATIC_SUBCMD_SET_CREATE(http_perso,
        SHELL_CMD_ARG(get, NULL, "Send HTTP request\n This command requires one argument : $ http get <file_relative_path>", http_request, 1, 1),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(http, &http_perso, "HTTP shell commands", NULL);


// Socket commands

static int socket_receive(const struct shell *sh,
                            size_t argc, char **argv)
{
    if(state_wifi){
        static int ret = 0;

        ret = Socket_Init();

        if(!ret){
            shell_print(sh, "Error during socket connection");
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

        return 1;
    }
    else{
        shell_print(sh, "Device not connected to wifi, please connect before using socket");
    }
}

static int socket_send(const struct shell *sh,
                            size_t argc, char **argv)
{
    if(state_wifi){
        if(argc==2){
            static int ret = 0;

            ret = Socket_Init();

            if(!ret){
                shell_print(sh, "Error during socket connection");
                Socket_Close();
                return 0;
            }
        
            ret = Socket_Send(argv[1]);

            if(!ret){
                shell_print(sh, "Error while receiving data");
                Socket_Close();
                return 0;
            }

            Socket_Close();

            return 1;
        }
        else{
            shell_print(sh, "PLease specify the data you want to send");
            return 0;
        }
    }
    else{
        shell_print(sh, "Device not connected to wifi, please connect before using socket");
    }
}

SHELL_STATIC_SUBCMD_SET_CREATE(socket_perso,
        SHELL_CMD_ARG(send, NULL, "Send data to a server\n This command requires one argument : socket send <data_to_send>", socket_send, 1, 1),
        SHELL_CMD_ARG(recv, NULL, "Receive data from a server", socket_receive, 1, 0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(socket, &socket_perso, "Socket shell commands", NULL);


// Credentials commands

static int cred_show(const struct shell *sh,
                            size_t argc, char **argv)
{
    char* cert;
    cert = flash_read_cert();
    shell_print(sh, "cert :",cert);
    return 1;
}

static int cred_update(const struct shell *sh,
                            size_t argc, char **argv)
{
    return 1;
}

SHELL_STATIC_SUBCMD_SET_CREATE(cred_perso,
        SHELL_CMD_ARG(show, NULL, "Show credentials", cred_show, 1, 1),
        SHELL_CMD_ARG(update, NULL, "Update credentials", cred_update, 1, 0),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(cred, &cred_perso, "Credentials shell commands", NULL);