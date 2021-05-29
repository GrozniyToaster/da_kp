#include "arithmetic_coding.hpp"
using namespace std;


const int ALPHABET_SIZE = UCHAR_MAX + 1;

template<typename Input, typename Var>
auto read_raw_var(Input& input, Var& v) -> decltype(input.read(reinterpret_cast<char*>(&v), sizeof(v))) {
    return input.read(reinterpret_cast<char*>(&v), sizeof(v));
}
template<typename Input>
auto read_raw_var(Input& input, vector<bool>& v) -> decltype(input.read(reinterpret_cast<char*>(&v), sizeof(unsigned char))) {
    auto size = v.size();
    if (!input.read(reinterpret_cast<char*>(&size), sizeof(size))) return input;
    v.reserve(size);
    auto real_size = (size / 8) + ((size % 8) ? 1 : 0);
    unsigned char one_byte;
    for (int i = 0; i < real_size - 1; i++) {
        input.read(reinterpret_cast<char*>(&one_byte), sizeof(one_byte));
        for (int j = 7; j >= 0; --j) {
            v.push_back( (one_byte >> j) & 1);
        }
        size -= 8;
    }
    input.read(reinterpret_cast<char*>(&one_byte), sizeof(one_byte));
    int back_border = 8 - size;
    for (int j = 7; j >= back_border; --j) {
        v.push_back( (one_byte >> j) & 1);
    }
    return input;
}

template<typename Input, typename Var>
auto write_raw_var(Input& input, const Var& v) -> decltype(input.write(reinterpret_cast<const char*>(&v), sizeof(v))) {
    return input.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

template<typename Input>
auto write_raw_var(Input& input, const vector<bool>& v) -> decltype(input.write(reinterpret_cast<const char*>(&v), sizeof(unsigned char))) {
    auto size = v.size();
    if (!input.write(reinterpret_cast<const char*>(&size), sizeof(size))) return input;
    unsigned char one_byte = 0;
    auto count_bytes = 0;
    for (int i = 0; i < size; ++i) {
        one_byte <<= 1;
        one_byte |= v[i];
        count_bytes++;
        if (count_bytes == 8) {
            input.write(reinterpret_cast<const char*>(&one_byte), sizeof(one_byte));
            count_bytes = 0;
        }
    }
    if (count_bytes) {
        one_byte <<= (8 - count_bytes);
        input.write(reinterpret_cast<const char*>(&one_byte), sizeof(one_byte));
    }
    return input;
}

byte get_byte(const vector<bool>& v, size_t start, size_t finish){
    byte res {0};
    for (int i = 0; i < min(finish - start, 8ul); i++){
        res <<= 1;
        res |= byte{v[start + i]};
    }
    return res;
}

struct Byte {
    uint8_t size = 0;
    byte code{0};
};

Byte operator<<(Byte b, int a) {
    if (a == 0) {
        b.code <<= 1;
        ++b.size;
    } else {
        b.code <<= 1;
        b.code |= static_cast<byte>(1);
        ++b.size;
    }
    return b;
}


vector<uint64_t> find_letters_probability(ifstream& file) {
    unsigned char ch;
    vector<uint64_t> probability(ALPHABET_SIZE, 0);
    uint64_t size = 0;
    while (read_raw_var(file, ch)) {
        probability[ch]++;
        size++;
    }
    return probability;
}
struct Node {
    Node* left = nullptr;
    Node* right = nullptr;
    unsigned char c = 0;
    Node(unsigned char c) : c(c) {}
    Node(Node* l, Node* r) : left(l), right(r) {}
};
struct Compare {
    bool operator()(pair<uint32_t, Node*> l, pair<uint32_t, Node*> r) {
        return l.first > r.first;
    }
};
map<unsigned char, vector<bool>> make_table(vector<uint64_t>& pr) {
    priority_queue<pair<uint32_t, Node*>, vector<pair<uint32_t, Node*>>, Compare> q;
    for (uint16_t i = 0; i < ALPHABET_SIZE; ++i) {
        if (pr[i] == 0) continue;
        Node* tmp = new Node(static_cast<unsigned char>(i));
        q.emplace(pr[i], tmp);
    }
    while (q.size() > 1) {
        auto [new_w, l_ptr] = q.top();
        q.pop();
        auto [w_r, r_ptr] = q.top();
        q.pop();
        auto new_node = new Node(l_ptr, r_ptr);
        new_w += w_r;
        q.emplace(new_w, new_node);
    }
    auto [max_w, root] = q.top();
    //TODO max_w == size
    queue<pair<Node*, vector<bool>>> dfs;
    dfs.emplace(root, vector<bool>());
    map<unsigned char, vector<bool>> res;
    while (!dfs.empty()) {
        auto [cur_node, code] = dfs.front();
        dfs.pop();
        if (cur_node->left == nullptr && cur_node->right == nullptr) {
            res[cur_node->c] = code;
        } else {
            //if (cur_node->left == nullptr || cur_node->right == nullptr){
                //throw "nonono";
            //}
            auto copy_code = code;
            copy_code.push_back(0);
            dfs.emplace(cur_node->left, move(copy_code));
            code.push_back(1);
            dfs.emplace(cur_node->right, move(code));
        }
        delete cur_node;
    }
    return res;
}

void update_and_write_byte(Byte& b, const vector<bool>& to_write, ostream& out) {
    if (b.size >= to_write.size()) {
        b.code <<= to_write.size();
        b.code |= get_byte(to_write, 0, to_write.size());
        b.size -= to_write.size();
    } else {
        b.code <<= b.size;
        b.code |= get_byte(to_write, 0, b.size);
        write_raw_var(out, b.code);
        for(size_t i = b.size; i < to_write.size(); i+= 8){
            if (i + 8 >= to_write.size()){
                b.code = get_byte(to_write, i, to_write.size());
                b.size = 8 - (to_write.size() - i);
                break;
            } else {
                b.code = get_byte(to_write, i,  i + 8);
                write_raw_var(out, b.code);
            }

        }

    }
    if (b.size == 0){
        write_raw_var(out, b.code);
        b.size = 8;
    }
}

void write_last_byte(Byte& b, ostream& out) {
    b.code <<= b.size;
    write_raw_var(out, b.code);
    write_raw_var(out, b.size);
}

void Huffman::encode(string& input, ostream& out) {
    ifstream first_watch(input, std::ios_base::in | std::ios::binary);
    ifstream second_watch(input, std::ios_base::in | std::ios::binary);
    auto probability = find_letters_probability(first_watch);
    auto table = make_table(probability);
    for (auto& [ch, v]: table){
        cout << ch << ' ';
        for (auto b: v){
            cout << b;
        }
        cout << '\n';
    }
    unsigned char c;
    Byte tbyte;
    tbyte.size = 8;// Available bits
    write_raw_var(out, static_cast<uint16_t>(table.size()));
    for (auto& [ch, code] : table) {
        write_raw_var(out, ch);
        write_raw_var(out, code);
    }
    while (read_raw_var(second_watch, c)) {
        update_and_write_byte(tbyte, table[c], out);
    }
    write_last_byte(tbyte, out);
}

map<unsigned char, vector<bool>> read_table(istream& input) {
    uint16_t size;
    read_raw_var(input, size);
    map<unsigned char, vector<bool>> res;
    unsigned char tc;
    vector<bool> tb;
    for (int i = 0; i < size; ++i) {
        read_raw_var(input, tc);
        read_raw_var(input, tb);
        res[tc] = move(tb);
    }
    return res;
}

Node* make_tree(map<unsigned char, vector<bool>>& table) {
    Node* root = new Node(nullptr, nullptr);
    for (auto& [ch, b] : table) {
        Node** cur = &root;
        for (auto && cur_bit : b) {
            if (*cur == nullptr) {
                *cur = new Node(nullptr, nullptr);
            }
            if (cur_bit) {
                cur = &((*cur)->right);
            } else {
                cur = &((*cur)->left);
            }
        }
        if (*cur == nullptr) {
            *cur = new Node(ch);
        }
    }
    queue<Node*> dfs;
    dfs.emplace(root);
    while (!dfs.empty()){
        auto c = dfs.front();
        if (c->left == nullptr && c->right == nullptr){

        } else if (c->left == nullptr || c->right == nullptr) {
            throw "wtf";
        } else {
            dfs.push(c->left);
            dfs.push(c->right);
        }
        dfs.pop();
    }
    return root;
}

void decode_byte(byte b, ostream& out, Node* tree, Node*& p, uint16_t back_offset = 0) {
    if (p->left == nullptr && p->right == nullptr) {
        write_raw_var(out, p->c);
        p = tree;
    }
    auto save = p;
    for (int i = 7; i >= back_offset; --i) {
        bool direction = ((b >> i) & byte{1}) == byte{1};
        p = (direction) ? p = p->right : p = p->left;
        if (p->left == nullptr && p->right == nullptr) {
            write_raw_var(out, p->c);
            p = tree;
        }
    }
    if (p->left == nullptr && p->right == nullptr) {
        write_raw_var(out, p->c);
        p = tree;
    }
}

void Huffman::decode(ifstream& input, ostream& out) {
    auto table = read_table(input);
    for (auto& [ch, v]: table){
        cout << int(ch) << ' ';
        for (auto b: v){
            cout << b;
        }
        cout << '\n';
    }
    auto tree = make_tree(table);
    queue<byte> q;
    byte tb;
    if (!read_raw_var(input, tb)) {
        //TODO some
    }
    q.push(tb);
    if (!read_raw_var(input, tb)) {
        //TODO some
    }
    q.push(tb);
    auto cur_state = tree;
    while (read_raw_var(input, tb)) {
        decode_byte(q.front(), out, tree, cur_state);
        q.pop();
        q.push(tb);
    }
    auto last_byte = q.front();
    q.pop();
    decode_byte(last_byte, out, tree, cur_state, to_integer<uint16_t>(q.front()));

    queue<Node*> to_delete;
    to_delete.push(tree);
    while (!to_delete.empty()) {
        auto cur = to_delete.front();
        to_delete.pop();
        if (cur != nullptr) {
            to_delete.push(cur->right);
            to_delete.push(cur->left);
            delete cur;
        }
    }
}
