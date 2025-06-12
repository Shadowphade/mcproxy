#ifndef DATATYPE_H
#define DATATYPE_H
#include <stdio.h>
struct VarInt {
    int num;
    int len; //NUMBER OF BYTES READ FOR VARINT
};

struct MinecraftRequestHeader {
    char* name;
    size_t n;
    int port;
    char* buffer;
};

struct VarInt readvarint(char* buffer);
struct MinecraftRequestHeader parserequest(char* buffer, int size);

#endif
