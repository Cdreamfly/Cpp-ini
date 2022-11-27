#include <iostream>
#include "Ini.hpp"

int main() {
    Ini ini;
    ini.Load("../in.ini");
    ini["ser"]["id"] = "192.168.10.3";
    std::cout<<ini<<std::endl;
    ini.Save("../out.ini");
    return 0;
}
