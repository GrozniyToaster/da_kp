//
// Created by ivan- on 08.05.2021.
//

#include "lzw.hpp"

using namespace std;

const size_t UPPER_BORDER = UINT16_MAX;
const size_t DEFAULT_DICT_SIZE = UINT8_MAX + 1;


class Trie {
    struct Node {
        const uint16_t code;
        map<uint8_t, unique_ptr<Node>> next;

        explicit Node(uint16_t c) : code(c) {}

        Node() : code(0) {}
    };

    struct State {
        Node *cur_node;

    };

    unique_ptr<Node> root;
    State st;

public:


    Trie() : root(make_unique<Node>()), st{root.get()} {}

    bool step(uint8_t ch) {
        auto &next = st.cur_node->next[ch];
        if (next == nullptr) {
            return false;
        } else {
            st.cur_node = next.get();
            return true;
        }
    }

    void add(uint8_t ch, uint16_t code) const {
        st.cur_node->next[ch] = make_unique<Node>(code);
    }

    [[nodiscard]] uint16_t get_code() const noexcept {
        return st.cur_node->code;
    }

    void state_to_root() {
        st.cur_node = root.get();
    }

};

Trie encode_dict() {
    Trie dict;
    for (uint16_t i = 0; i <= UINT8_MAX; i++) {
        dict.add(i, i);
    }
    return dict;
}

void LZW::encode(istream &input, ostream &out) {
    auto dict = encode_dict();
    size_t count = DEFAULT_DICT_SIZE;
    char cur_ch;
    if (!read_raw_var(input, cur_ch)) {
        return;
    }
    dict.step(cur_ch);
    while (true) {
        if (!read_raw_var(input, cur_ch)) {
            break;
        }
        if (dict.step(cur_ch)) {
            continue;
        } else {
            write_raw_var(out, dict.get_code());
            dict.add(cur_ch, count);
            count++;
            if (count == UPPER_BORDER) {
                dict = encode_dict();
                count = DEFAULT_DICT_SIZE;
            }
            dict.state_to_root();
            dict.step(cur_ch);
        }
    }
    write_raw_var(out, dict.get_code());
}

vector<string> decode_dict() {
    vector<string> dict(DEFAULT_DICT_SIZE);
    for (uint16_t i = 0; i <= UINT8_MAX; i++) {
        dict[i] = static_cast<char>(i);
    }
    return dict;
}

void LZW::decode(istream &input, ostream &out) {
    auto dict = decode_dict();
    size_t count = dict.size();

    uint16_t old_code;
    if (!read_raw_var(input, old_code)) return;
    write_raw_var(out, dict[old_code]);
    char symbol = dict[old_code][0];
    uint16_t new_code;
    string str;
    while (read_raw_var(input, new_code)) {

        if (count <= new_code) {
            str = dict[old_code];
            str += symbol;

        } else {
            str = dict[new_code];
        }
        write_raw_var(out, str);
        symbol = str[0];
        dict.emplace_back(dict[old_code] + symbol);
        count++;
        if (count == UPPER_BORDER) {
            dict = decode_dict();
            count = dict.size();
        }
        old_code = new_code;
    }

}