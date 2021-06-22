//
// Created by ivan- on 08.05.2021.
//

#include "lzw.hpp"
using namespace std;

const uint16_t UPPER_BORDER = UINT16_MAX;
const uint16_t EOF_CODE = 26;

unordered_map<string, uint16_t> encode_dict() {
    unordered_map<string, uint16_t> dict;
    uint16_t count = 0;
    for (unsigned char i = 0; i < UINT8_MAX; i++) {
        dict[string(1, i + 1)] = count;
        count++;
    }
    // dict[string(1, '\0')] = EOF_CODE;
    return dict;
}

void LZW::encode(istream& input, ostream& out) {
    auto dict = encode_dict();
    uint16_t count = dict.size();
    char cur_ch;
    if (!read_raw_var(input, cur_ch)) {
        //cur_ch = '\0';
        return;
    }
    string cur_str(1, cur_ch);
    while (true) {
        bool to_break = false;
        if (!read_raw_var(input, cur_ch)) {
            //cur_ch = '\0';
            to_break = true;
        }
        if (dict.count(cur_str + cur_ch)) {
            cur_str += cur_ch;
        } else {
            out << dict[cur_str] << ' ';
            dict[cur_str + cur_ch] = count;
            count++;
            if (count == UPPER_BORDER) {
                dict = encode_dict();
                count = dict.size();
            }
            cur_str = cur_ch;
        }
        if (to_break) {
            break;
        }
    }
    out << dict[cur_str];
}

unordered_map<uint16_t, string> decode_dict() {
    uint16_t count = 0;
    unordered_map<uint16_t, string> dict;
    for (unsigned char i = 0; i < UINT8_MAX; i++) {
        dict[count] = i + 1;
        count++;
    }
    // dict[EOF_CODE] = '\0';
    return dict;
}

void LZW::decode(istream& input, ostream& out) {
    auto dict = decode_dict();
    uint16_t count = dict.size();

    uint16_t old_code;
    if (!(input >> old_code)) return;
    out << dict[old_code];
    char symbol = dict[old_code][0];
    uint16_t new_code;
    while (input >> new_code) {
        //if (new_code == EOF_CODE) {
        //    break;
        //}
        string str;
        if (!dict.count(new_code)) {
            str = dict[old_code];
            str += symbol;

        } else {
            str = dict[new_code];
        }
        out << str;
        symbol = str[0];
        dict[count] = dict[old_code] + symbol;
        count++;
        if (count == UPPER_BORDER) {
            dict = decode_dict();
            count = dict.size();
        }
        old_code = new_code;
    }
}