// Copyright (c) 2021.

#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <climits>
#include <map>
#include <queue>
#include <cstddef>
namespace Huffman {
    void encode(std::string& input, std::ostream& out);
    void decode(std::ifstream& input, std::ostream& out);
}
