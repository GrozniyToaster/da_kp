#include "arithmetic_coding.hpp"
using namespace std;

using ld = long double;
const int ALPHABET_SIZE = UCHAR_MAX;

template <typename Input, typename Var>
auto read_raw_var(Input& input, Var& v) -> decltype(input.read(reinterpret_cast<char*>(&v), sizeof(v))){
    return input.read(reinterpret_cast<char*>(&v), sizeof(v));
}

template <typename Input, typename Var>
auto write_raw_var(Input& input, Var& v) -> decltype(input.write(reinterpret_cast<char*>(&v), sizeof(v))){
    return input.write(reinterpret_cast<char*>(&v), sizeof(v));
}

struct Segment {
  ld left;
  ld right;
};





vector<ld> find_letters_probability(ifstream& file){
    char ch;
    vector<ld> probability;
    vector<uint64_t> count (ALPHABET_SIZE, 0);
    uint64_t size = 0;
    while (file >> ch){
        count[ch]++;
        size++;
    }
    if (size == 0) return probability;
    probability.resize(ALPHABET_SIZE);
    for (int i = 0; i < ALPHABET_SIZE; ++i){
        probability[i] = static_cast<ld> (count[i]) / size;
    }
    return probability;

}

vector<Segment> make_segment(vector<ld>& pr){
    vector<Segment> s(ALPHABET_SIZE);
    ld l = 0;
    for (int i = 0; i < ALPHABET_SIZE; ++i){
        s[i].left = l;
        s[i].right = l + pr[i];
        l = s[i].right;
    }
    return s;
}

void Arithmetic::encode(string& input, ostream& out){
    ifstream first_watch (input);
    ifstream second_watch (input);
    auto probability = find_letters_probability(first_watch);
    auto segments = make_segment(probability);
    ld left = 0;
    ld right = 1;
    char ch;
    uint64_t size = 0;
    while (second_watch >> ch){
        ++size;
        ld tmp_right = (right - left) * segments[ch].right + left;
        ld tmp_left = (right - left) * segments[ch].left + left;
        right = tmp_right;
        left = tmp_left;
    }
    uint32_t count_non_zero_pr = 0;
    for (int i = 0; i < ALPHABET_SIZE; ++i){
        if (probability[i] == 0) continue;
        count_non_zero_pr++;
    }
    out.write(reinterpret_cast<char*>(&count_non_zero_pr), sizeof(count_non_zero_pr));
    for (unsigned char i = 0; i < ALPHABET_SIZE; ++i){
        if (probability[i] == 0) continue;
        out.write( reinterpret_cast<char*>(&i), sizeof(i));
        out.write(reinterpret_cast<char*>(&probability[i]), sizeof(ld));
    }
    ld res = (right + left) / 2;
    out.write(reinterpret_cast<char*>(&size), sizeof(size));
    out.write(reinterpret_cast<char*>(&res), sizeof(res));
}


vector<ld> read_probability(ifstream& input){
    vector<ld> pr (ALPHABET_SIZE, 0);
    uint32_t count;
    read_raw_var(input, count) ;
    char ch;
    ld p;
    for (int i = 0; i < count; ++i){
        read_raw_var(input, ch);
        read_raw_var(input, p);
        pr[ch] = p;
    }
    return pr;
}

vector<Segment> make_decode_segments (vector<ld>& pr){
    vector<Segment> res (ALPHABET_SIZE);
    ld l = 0;
    for (int i = 0; i < ALPHABET_SIZE; ++i){
        res[i].left = l;
        res[i].right = l + pr[i];
        l = res[i].right;
    }
    return res;
}

void Arithmetic::decode(ifstream& input, ostream& out){
    auto probability = read_probability(input);
    auto segments = make_decode_segments(probability);
    uint64_t size;
    ld code;
    read_raw_var(input, size);
    read_raw_var(input, code);
    for (uint64_t s = 0; s < size; ++s){
        for (unsigned char i = 0; i < ALPHABET_SIZE; ++i){
            if (segments[i].left <= code && code <= segments[i].right ){
                write_raw_var(out, i);
                code = (code - segments[i].left) / (segments[i].right - segments[i].left);
                break;
            }
        }
    }
}



