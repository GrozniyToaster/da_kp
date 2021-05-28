//
// Created by ivan- on 08.05.2021.
//
#pragma once
#include <unordered_map>
#include <vector>
#include <iostream>


namespace LZW {
    void encode(std::istream& input, std::ostream& out);
    void decode(std::istream& input, std::ostream& out);
}


