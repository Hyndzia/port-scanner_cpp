#include <iostream>
#include <memory>
#include "scanner.h"

int main(){
    auto scanner{std::make_unique<Scanner>()};
    scanner->scanUI();
    return 0;
}