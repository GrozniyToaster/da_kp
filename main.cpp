//
// Created by ivan- on 08.05.2021.
//

#include "arithmetic_coding.hpp"
#include "lzw.hpp"
#include <iostream>
#include <sstream>
int main() {
    std::string comand;
    std::cin >> comand;
    if (comand == "compress") {
        /*
      std::string s;
    std::stringstream ss;
    std::cin >> s;
    ss << s;
    LZW::encode(ss, std::cout);
    std::cout << '\n';
    return 0;
     */
        std::string filename, to_save;
        std::cin >> filename >> to_save;
        std::ofstream s(to_save,  std::ios::out | std::ios::binary );
        Arithmetic::encode(filename, s);
    } else if (comand == "decompress") {
        std::string filename, to_save;
        std::cin >> filename;
        std::ifstream f (filename);
        Arithmetic::decode(f, std::cout);
        std::cout << '\n';
        return 0;
    } else {
        return 1;
    }
}