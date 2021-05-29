//
// Created by ivan- on 08.05.2021.
//
#include "huffman.hpp"
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
        Huffman::encode(filename, s);
    } else if (comand == "decompress") {
        std::string filename, to_save;
        std::cin >> filename >> to_save;
        std::ifstream f (filename, std::ios_base::in | std::ios::binary);
        std::ofstream s (to_save, std::ios_base::out |std::ios::binary);
        Huffman::decode(f, s);
        std::cout << '\n';
        return 0;
    } else {
        return 1;
    }
}