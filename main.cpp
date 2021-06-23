#include "huffman.hpp"
#include "lzw.hpp"
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>
#include <cctype>
#include "utils.hpp"

using namespace std;

enum Opt {
    Cin,
    Cout,
    Decompress,
    Recursion,
    CompressionLevel,
    Test,
    BadOption,
    Count

};

enum class Algorithm{
    LZW,
    Huffman
};

void check_logics_opts(vector<bool> &opt) {
    if (opt[Opt::BadOption]) return;
    if (opt[Opt::Recursion]) {
        if (opt[Opt::Cin] || opt[Opt::Cout]) {
            opt[Opt::BadOption] = true;
        }
    }
}

tuple<vector<bool>, vector<string>> parse_opt(int argc, char *argv[]) {
    vector<bool> opt(Opt::Count, false);
    vector<string> args(2);
    int real_args = 0;
    for (int i = 1; i < argc; ++i) {
        string cur_opt = argv[i];
        if (cur_opt[0] == '-') {
            if (cur_opt.size() > 2) {
                opt[Opt::BadOption] = true;
            } else if (cur_opt.size() == 1) {
                opt[Opt::Cin] = true;
            } else {
                switch (cur_opt[1]) {
                    case 'c':
                        opt[Opt::Cout] = true;
                        break;
                    case 'd':
                        opt[Opt::Decompress] = true;
                        break;
                    case 'r':
                        opt[Opt::Recursion] = true;
                        break;
                    case 't':
                        opt[Opt::Test] = true;
                        break;
                    default:
                        if (isdigit(cur_opt[1])) {
                            opt[Opt::CompressionLevel] = true;
                            args[1] = cur_opt[1];
                        } else {
                            opt[Opt::BadOption] = true;
                        }
                }
            }

        } else {
            if (real_args >= 1) {
                opt[Opt::BadOption] = true;
                continue;
            }
            args[real_args] = argv[i];
            ++real_args;
        }
    }
    if (real_args != 1) opt[Opt::BadOption] = true;
    check_logics_opts(opt);
    return {opt, args};
}

void one_file(vector<bool> &opt, vector<string> arg) {
    if (opt[Opt::Decompress]) {
        //TODO
    } else {
        if (opt[Opt::Cin] || (opt[Opt::CompressionLevel] && arg[1][0] <= '5')) {
            if (opt[Opt::Cin]) {
                if (opt[Opt::Cout]){
                    write_raw_var(cout, static_cast<uint8_t>(Algorithm::LZW));
                    LZW::encode(std::cin, cout);
                } else {
                    std::ofstream output("input.zipped", std::ios::out | std::ios::binary);
                    write_raw_var(output, static_cast<uint8_t>(Algorithm::LZW));
                    LZW::encode(std::cin, output);
                }
            } else {
                if (opt[Opt::Cout]){
                    std::ifstream input(arg[0]);
                    write_raw_var(cout, static_cast<uint8_t>(Algorithm::LZW));
                    LZW::encode(input, cout);
                } else {
                    std::ifstream input(arg[0]);
                    std::ofstream output(arg[0] + ".zipped", std::ios::out | std::ios::binary);
                    write_raw_var(output, static_cast<uint8_t>(Algorithm::LZW));
                    LZW::encode(input, output);
                }
            }
        } else {
            if (opt[Opt::Cout]){
                write_raw_var(cout, static_cast<uint8_t>(Algorithm::Huffman));
                Huffman::encode(arg[0], cout);
            } else {
                std::ofstream output(arg[0] + ".zipped", std::ios::out | std::ios::binary);
                write_raw_var(output, static_cast<uint8_t>(Algorithm::Huffman));
                Huffman::encode(arg[0], output);
            }
        }
    }

}

int main(int argc, char *argv[]) {
    auto[opt, arg] = parse_opt(argc, argv);
    if (opt[Opt::Cin] || (Opt::CompressionLevel && arg[1][0] <= '5'))

    /*
    std::string comand;
    std::cin >> comand;
    if (comand == "compress") {

        std::string filename, to_save;
        std::cin >> filename >> to_save;
        std::ifstream i(filename);
        std::ofstream s(to_save,  std::ios::out | std::ios::binary );
        LZW::encode(i, s);
    } else if (comand == "decompress") {
        std::string filename, to_save;
        std::cin >> filename >> to_save;
        std::ifstream f (filename, std::ios_base::in | std::ios::binary);
        std::ofstream s (to_save, std::ios_base::out |std::ios::binary);
        LZW::decode(f, s);
        std::cout << '\n';
        return 0;
    } else {
        return 1;
    }*/
}