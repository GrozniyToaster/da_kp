//
// Created by ivan- on 08.05.2021.
//

#include "lzw.hpp"

using namespace std;

const uint16_t UPPER_BORDER = UINT16_MAX;


class Trie {
    struct Node {
        const uint16_t code;
        map<uint8_t, unique_ptr<Node>> next;

        explicit Node(uint16_t c) : code(c) {}
        Node(): code(0) {}
    };

    struct State{
        Node* cur_node;

    };

    unique_ptr<Node> root;
    State st;

public:


    Trie() : root(make_unique<Node>()), st{root.get()} {}
    bool step(uint8_t ch){
        auto& next =  st.cur_node->next[ch];
        if (next == nullptr){
            return false;
        } else {
            st.cur_node = next.get();
            return true;
        }
    }
    void  add (uint8_t ch, uint16_t code){
        st.cur_node->next[ch] = make_unique<Node>(code);
    }

    uint16_t get_code (){
        return st.cur_node->code;
    }
    void state_to_root(){
        st.cur_node = root.get();
    }

};
/*
unordered_map<string, uint16_t> encode_dict() {
    unordered_map<string, uint16_t> dict;
    uint16_t count = 0;
    for (unsigned char i = 0; i < UINT8_MAX; i++) {
        dict[string(1, i)] = count;
        count++;
    }
    dict[string(1, uint8_t(UINT8_MAX))] = count;

    // dict[string(1, '\0')] = EOF_CODE;
    return dict;
}
*/
Trie encode_dict() {
    Trie dict;
    for (uint8_t i = 0; i < UINT8_MAX; i++) {
        dict.add(i, i);
    }
    dict.add(UINT8_MAX, UINT8_MAX);
    return dict;
}

void LZW::encode(istream &input, ostream &out) {
    auto dict = encode_dict();
    uint16_t count = UINT8_MAX + 1;
    char cur_ch;
    if (!read_raw_var(input, cur_ch)) {
        //cur_ch = '\0';
        return;
    }
    // string cur_str(1, cur_ch);
    dict.step(cur_ch);
    while (true) {
        bool to_break = false;
        if (!read_raw_var(input, cur_ch)) {
            break;
        }
        if (dict.step(cur_ch)) {
            //cur_str += cur_ch;
            continue;
        } else {
            write_raw_var(out, dict.get_code());
            //dict[cur_str + cur_ch] = count;
            dict.add(cur_ch, count);
            count++;
            if (count == UPPER_BORDER) {
                dict = encode_dict();
                count = UINT8_MAX + 1;
            }
            //cur_str = cur_ch;
            dict.state_to_root();
            dict.step(cur_ch);
        }
    }
    write_raw_var(out, dict.get_code());
}

unordered_map<uint16_t, string> decode_dict() {
    uint16_t count = 0;
    unordered_map<uint16_t, string> dict;
    for (unsigned char i = 0; i < UINT8_MAX; i++) {
        dict[count] = i;
        count++;
    }
    dict[count] = UINT8_MAX;
    // dict[EOF_CODE] = '\0';
    return dict;
}

void LZW::decode(istream &input, ostream &out) {
    auto dict = decode_dict();
    uint16_t count = dict.size();

    uint16_t old_code;
    if (!read_raw_var(input, old_code)) return;
    write_raw_var(out, dict[old_code]);
    char symbol = dict[old_code][0];
    uint16_t new_code;
    string str;
    while (read_raw_var(input, new_code)) {

        if (!dict.count(new_code)) {
            str = dict[old_code];
            str += symbol;

        } else {
            str = dict[new_code];
        }
        write_raw_var(out, str);
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