#include "huffman.hpp"
#include "lzw.hpp"
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>
#include <cctype>
#include <regex>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
enum Opt {
    Cin,
    Cout,
    Decompress,
    Recursion,
    CompressionLevel,
    Test,
    Statistics,
    DontDelete,
    BadOption,
    Count

};

bool check_suffix(const string &str) {
    const string right_suffix = ".zipped";
    auto shift = right_suffix.size();
    if (str.size() < shift) return false;
    auto substr = str.substr(str.size() - shift, shift);
    return substr == right_suffix;
}

enum class Algorithm {
    LZW,
    Huffman
};

void one_file(vector<bool> &opt, vector<string> &arg);

void work_with_one_file(vector<bool> &opt, vector<string> &arg){
    try{
        one_file(opt, arg);
    } catch (...) {
        cout << "file " << arg[0] << " was corrupted\n";
        return;
    }

    if (opt[Opt::Statistics] && !opt[Opt::Decompress] && !opt[Opt::Test]){
        auto size_normal = fs::file_size(arg[0]);
        auto size_compressed = fs::file_size(arg[0] + ".zipped");
        auto coef = static_cast<double>(size_compressed) / size_normal;
        cout << "file: " << arg[0] << "\n\tsize: " << size_normal << '\n'
             << "compressed file: " << arg[0] << ".zipped\n\tsize: " << size_compressed << '\n'
             << "coefficient compress: " << coef << '\n';
    }
    if (!opt[Opt::DontDelete] && !opt[Opt::Cin] && !opt[Opt::Test]){
        try {
            fs::remove(arg[0]);
        } catch (...){
            cout << "Cant delete " << arg[0] <<'\n';
        }
    }
    if (opt[Opt::Test]){
        cout << "Archive " << arg[0] << " seems good\n";
    }
}

void check_logics_opts(vector<bool> &opt, vector<string> &arg) {
    if (opt[Opt::BadOption]) return;
    if (!fs::exists(arg[0])) test.zippedopt[Opt::BadOption] = true;
    if (opt[Opt::Test]) opt[Opt::Decompress] = true;
    if (opt[Opt::Recursion]) {
        if (opt[Opt::Cin] || opt[Opt::Cout]) {
            opt[Opt::BadOption] = true;
        }
        if (!fs::is_directory(arg[0])) {
            opt[Opt::BadOption] = true;
        }
        return;
    }
    if (!opt[Opt::Cin] && (opt[Opt::Decompress] || opt[Opt::Test])) {
        if (fs::is_directory(arg[0])) {
            opt[Opt::BadOption] = true;
            return;
        }
        if (!check_suffix(arg[0])) {
            opt[Opt::BadOption] = true;
            return;
        }
    }
    if (fs::is_directory(arg[0])) {
        opt[Opt::BadOption] = true;
        return;
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
                    case 'l':
                        opt[Opt::Statistics] = true;
                        break;
                    case 'k':
                        opt[Opt::DontDelete] = true;
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
    if (real_args != 1 && !opt[Opt::Cin]) opt[Opt::BadOption] = true;
    check_logics_opts(opt, args);
    return {opt, args};
}

void one_file(vector<bool> &opt, vector<string> &arg) {
    if (opt[Opt::Decompress] || opt[Opt::Test]) {
        if (opt[Opt::Cin]) {
            if (opt[Opt::Cout]) {
                uint8_t algo;
                read_raw_var(cin, algo);
                if (static_cast<uint8_t>(Algorithm::LZW) == algo) {
                    LZW::decode(cin, cout);
                } else {
                    throw "Error";
                }
            } else {
                uint8_t algo;
                std::ofstream out("input.unpacked");
                read_raw_var(cin, algo);
                if (static_cast<uint8_t>(Algorithm::LZW) == algo) {
                    LZW::decode(cin, out);
                } else {
                    throw "Error";
                }
            }
        } else {
            ifstream input(arg[0]);
            if (opt[Opt::Cout]) {
                uint8_t algo;
                read_raw_var(cin, algo);
                if (static_cast<uint8_t>(Algorithm::LZW) == algo) {
                    LZW::decode(input, cout);
                } else if (static_cast<uint8_t>(Algorithm::Huffman) == algo) {
                    Huffman::decode(input, cout);
                } else {
                    throw "Error";
                }
            } else {
                uint8_t algo;
                string out_file = (opt[Opt::Test])? "/dev/null" : arg[0] + ".unpacked";
                std::ofstream out(out_file);
                read_raw_var(input, algo);
                if (static_cast<uint8_t>(Algorithm::LZW) == algo) {
                    LZW::decode(input, out);
                } else if (static_cast<uint8_t>(Algorithm::Huffman) == algo) {
                    Huffman::decode(input, out);
                } else {
                    throw "Error";
                }
            }
        }
    } else {
        if (opt[Opt::Cin] || (opt[Opt::CompressionLevel] && arg[1][0] <= '5')) {
            if (opt[Opt::Cin]) {
                if (opt[Opt::Cout]) {
                    write_raw_var(cout, static_cast<uint8_t>(Algorithm::LZW));
                    LZW::encode(std::cin, cout);
                } else {
                    std::ofstream output("input.zipped", std::ios::out | std::ios::binary);
                    auto algo = static_cast<uint8_t>(Algorithm::LZW);
                    output.write(reinterpret_cast<char *>(&algo), sizeof(algo));
                    LZW::encode(std::cin, output);
                }
            } else {
                if (opt[Opt::Cout]) {
                    std::ifstream input(arg[0]);
                    write_raw_var(cout, static_cast<uint8_t>(Algorithm::LZW));
                    LZW::encode(input, cout);
                } else {
                    std::ifstream input(arg[0]);
                    std::ofstream output(arg[0] + ".zipped", std::ios::out | std::ios::binary);
                    auto algo = static_cast<uint8_t>(Algorithm::LZW);
                    output.write(reinterpret_cast<char *>(&algo), sizeof(algo));
                    LZW::encode(input, output);
                }
            }
        } else {
            if (opt[Opt::Cout]) {
                write_raw_var(cout, static_cast<uint8_t>(Algorithm::Huffman));
                Huffman::encode(arg[0], cout);
            } else {
                std::ofstream output(arg[0] + ".zipped", std::ios::out | std::ios::binary);
                auto algo = static_cast<uint8_t>(Algorithm::Huffman);
                output.write(reinterpret_cast<char *>(&algo), sizeof(algo));
                Huffman::encode(arg[0], output);
            }
        }
    }

}

void recursion(vector<bool> &opt, vector<string> &arg) {
    vector<string> to_do;
    if (opt[Opt::Decompress]) {
        for (const auto &entry : fs::recursive_directory_iterator(arg[0])) {
            if (check_suffix(entry.path())) {
                to_do.emplace_back(entry.path());
            }
        }
    } else {

        for (const auto &entry : fs::recursive_directory_iterator(arg[0])) {
            if (!fs::is_directory(entry.path())) {
                to_do.emplace_back(entry.path());
            }

        }
    }
    for (auto &td: to_do) {
        vector<string> one_file_arg{move(td), arg[1]};
        work_with_one_file(opt, one_file_arg);
    }

}

int main(int argc, char *argv[]) {
    auto[opt, arg] = parse_opt(argc, argv);
    try{
    if (opt[Opt::BadOption]) {
        cout << "Bad options\n";
        return 0;
    }
    if (opt[Opt::Recursion]) {
        recursion(opt, arg);
    } else {
        work_with_one_file(opt, arg);
    }
    } catch (...) {
        cout << "app stopped by unexpected problem\n";
    }

}