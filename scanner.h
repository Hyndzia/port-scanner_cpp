#pragma once
#include <iostream>

class Scanner{
public:
    Scanner() : found(0){
    };
    bool isPortOpen(uint16_t port);
    void scanPorts();
    std::string resolveDNS();
    void scanUI();
private:
    char host[100];
    std::string ipv4;
    uint16_t start;
    uint16_t end;
    uint16_t found;

};
