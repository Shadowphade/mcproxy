#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
struct ServerList* parseconfig() {
    FILE * fileptr = fopen("Server.conf", "r");

    struct ServerList* output = malloc(sizeof(struct ServerList));
    
    char character;
    int filesize = 0;
    while(character != EOF) {
        character = fgetc(fileptr);
        filesize++;
    }
    char* filedata = malloc(sizeof(char) * filesize + 1);
    memset(filedata, '\0', filesize + 1);
    rewind(fileptr);
    for(int i = 0; i < filesize; i++) {
        filedata[i] = fgetc(fileptr);
    }
    char curChar;
    char* token[1024]; // file cap, could be raised easily
    size_t token_n = 0;

    for(int i = 0; i < filesize; i++) {
        curChar = filedata[i];
        if (curChar == '#') {
            for (int j = i; j < filesize; j++) {
                if (filedata[j] == '\n') {
                    i = j;
                    break;
                }
            }

        }
        if (curChar == '[') {
            for (int j = i; j < filesize; j++) {
                if (filedata[j] == ']') {
                    token[token_n] = filedata + i;
                    token_n++;
                    break;
                }
            }
        }
        if (curChar == ' ' || curChar == '\n') {
            token[token_n] = filedata + i + 1;
            token_n++;
            filedata[i] = '\0';
        }
    }

    output->server_list = malloc(sizeof(Server) * token_n);
    output->server_list_n = 0;

    regex_t ipaddrreg;
    regex_t actionlabel;

    int regerr = regcomp(&ipaddrreg, "([0-9]{1,3}\.){2}[0-9]{1,3}", REG_EXTENDED);
    if (regerr != 0) {printf("ERROR COMPILING REGULAR EXPRESSION");}
    regerr = regcomp(&actionlabel, "\[\w{1,}\]", REG_EXTENDED);
    if (regerr != 0) {printf("ERROR COMPILING REGULAR EXPRESSION");}

    for (int i = 0; i < token_n; i++) {
        //printf("%s\n", token[i]);
        if (regexec( &actionlabel, token[i], 0, NULL, 0) != 0) {
            if(strcmp(token[i], "[Default]") == 0) {
                printf("Regex Passed\n");
                Server default_server;
                struct Address default_server_addr = parseipaddress(token[i + 1]);
                default_server.addr = default_server_addr;
                default_server.hostname = "Default\0";
                default_server.hostname_n = 7;
                //printServer(default_server);
                output->default_server = default_server;
                i++;
                continue;
            }
        }
        if (regexec( &ipaddrreg, token[i], 0, NULL, 0) == 0 && i != 0) {
            Server newServer;

            struct Address newServer_addr = parseipaddress(token[i]);
            newServer.addr = newServer_addr;
            newServer.hostname = malloc(sizeof(char) * strlen(token[i - 1]));
            strcpy(newServer.hostname, token[i - 1]);
            newServer.hostname_n = strlen(token[i - 1]);

            //printServer(newServer);

            output->server_list[output->server_list_n] = newServer;
            output->server_list_n++;
        };
    }
    free(filedata);
    filedata = NULL;

    printServerList(*output);

    return output;
}

void printAddress(struct Address input) {
    printf("Address\nIP: %s\t Len: %d\n", input.ip_addr, input.port);
};

void printServer(Server input) {
    printf("Server\n Hostname: %s\n Server", input.hostname);
    printAddress(input.addr);
};

void printServerList(struct ServerList input) {
    printf("Server List\n Default Server:");
    printServer(input.default_server);
    for(int i=0; i < input.server_list_n; i++) {
        printf("Server %d\n", i);
        printServer(input.server_list[i]);
    }
};

struct Address parseipaddress(char* txtaddr) {
    struct Address output;
    char numbertxt[10] = { '\0' };
    size_t addr_len = 0;
    for (int i = 0; i < 14; i++) {
        printf("%c", txtaddr[i]);
        if(txtaddr[i] == ':') {
            strncpy(numbertxt, txtaddr + i + 1, 10);
            txtaddr[i] = '\0';
            addr_len = i;
            printf("\n");
            break;
        }
    }

    output.ip_addr = malloc(sizeof(char) * addr_len + 1);
    memset(output.ip_addr, '\0', addr_len + 1);

    for(int i = 0; i < addr_len; i++) {
        output.ip_addr[i] = txtaddr[i];
    }
    printf("PORT STRING: %s\n", numbertxt);
    output.port = atoi(numbertxt);

    return output;
}
