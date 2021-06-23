#pragma once

#include <iostream>
#include <vector>


template<typename Input, typename Var>
auto read_raw_var(Input& input, Var& v) -> decltype(input.read(reinterpret_cast<char*>(&v), sizeof(v)));

template<typename Input>
Input& read_raw_var(Input& input, std::vector<bool>& v);

template<typename Output, typename Var>
Output& write_raw_var(Output& output, const Var& v);

template<typename Output>
Output& write_raw_var(Output& output, const std::vector<bool>& v);

template<typename Output>
Output& write_raw_var(Output& output, const std::string& s);


template<typename Input, typename Var>
auto read_raw_var(Input& input, Var& v) -> decltype(input.read(reinterpret_cast<char*>(&v), sizeof(v))) {
    return input.read(reinterpret_cast<char*>(&v), sizeof(v));
}
template<typename Input>
Input& read_raw_var(Input& input, std::vector<bool>& v) {
    uint16_t size;
    if (!read_raw_var(input, size)) return input;
    v.reserve(size);
    auto real_size = (size / 8) + ((size % 8) ? 1 : 0);
    unsigned char one_byte;
    for (int i = 0; i < real_size - 1; i++) {
        read_raw_var(input, one_byte);
        for (int j = 7; j >= 0; --j) {
            v.push_back((one_byte >> j) & 1);
        }
        size -= 8;
    }
    read_raw_var(input, one_byte);;
    int back_border = 8 - size;
    for (int j = 7; j >= back_border; --j) {
        v.push_back((one_byte >> j) & 1);
    }
    return input;
}

template<typename Output, typename Var>
Output& write_raw_var(Output& output, const Var& v) {
    return output.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

template<typename Output>
Output& write_raw_var(Output& output, const std::vector<bool>& v){
    uint16_t size = v.size();
    if (! write_raw_var( output, size)) return output;
    unsigned char one_byte = 0;
    auto count_bytes = 0;
    for (int i = 0; i < size; ++i) {
        one_byte <<= 1;
        one_byte |= v[i];
        count_bytes++;
        if (count_bytes == 8) {
            write_raw_var(output, one_byte);
            count_bytes = 0;
        }
    }
    if (count_bytes) {
        one_byte <<= (8 - count_bytes);
        write_raw_var(output, one_byte);
    }
    return output;
}

template<typename Output>
Output& write_raw_var(Output& output, const std::string& s){
    for (auto ch : s){
        if (!write_raw_var(output, ch)) return output;
    }
    return output;
}
