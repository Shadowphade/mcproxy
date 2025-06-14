#include "../include/proxy.h"
#include "../include/parser.h"

int main(int argc, char** argv) {
    struct ServerList* server_list = parseconfig();
    int servererr = startproxy(25565, server_list);
}
