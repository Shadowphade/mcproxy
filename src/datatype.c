#include "../include/datatype.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

struct VarInt readvarint(char* buffer) {
    struct VarInt output;

    int value = 0;
    int position = 0;
    unsigned char currentByte;

    int iterateor = 0; // CAUTION THIS WILL ONLY WORK IF WE ASSUME THAT THE INCOMING BUFFER STARTS WITH A VARINT;

    while (1) {
        currentByte = buffer[iterateor];
        iterateor++;
        value |= (currentByte & 0x7F) << position;
        if ((currentByte & 0x80) == 0) {
            break;
        }

        position += 7;

        if(position >= 32) {
            return output;
        }

    }

    output.num = value;
    output.len = iterateor;

    return output;
};

struct MinecraftRequestHeader parserequest(char* buffer, int size) {
    struct MinecraftRequestHeader output;
    int bufferpos = 0;
    struct VarInt Len = readvarint(buffer);
    bufferpos += Len.len;
    struct VarInt PacketID = readvarint(buffer + bufferpos);
    bufferpos += PacketID.len;
    struct VarInt Version = readvarint(buffer + bufferpos);
    bufferpos += Version.len;
    struct VarInt ServerStringLen = readvarint(buffer + bufferpos);
    bufferpos += ServerStringLen.len;
    char* serverstring = malloc(sizeof(char) * ServerStringLen.num + 1);
    memset(serverstring, '\0', ServerStringLen.num + 1);
    for(int i = 0; i < ServerStringLen.num; i++) serverstring[i] = buffer[bufferpos + i];
    bufferpos += ServerStringLen.num;
    unsigned short port;
    memcpy(&port, buffer+bufferpos,2);

    output.name = serverstring;
    output.n = ServerStringLen.len;
    output.port = ntohs(port);

    char* newbytebuffer = malloc(sizeof(char) * Len.num);
    for(int i = 0; i < Len.num; i++) newbytebuffer[i] = buffer[i];
    output.buffer = newbytebuffer;

    return output;
}
