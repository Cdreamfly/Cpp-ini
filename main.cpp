#include <iostream>
#include "Ini.hpp"

int main() {
    cmf::Ini ini;
    ini.load("../in.ini");
    ini["ser"]["id"] = "192.168.10.3";
    ini["server"]["id"] = "192.168.10.3";
    std::cout<<ini<<std::endl;
    ini.save("../out.ini");
    return 0;
}
