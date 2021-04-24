#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "needleman-wunsch.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);

    // File Parsing.
//    std::fstream compare_one, compare_two;
//    if (argc != 3) {
//        std::cout << "Warning: Using default values!";
//        compare_one.open("mouse-hemoglobin-sequence.fasta");
//        compare_two.open("human-hemoglobin-sequence.fasta");
//    } else {
//        compare_one.open(argv[1]);
//        compare_one.open(argv[2]);
//    }
//
//    if (!compare_one || !compare_two) {
//        std::cerr << "Error (2): Necessary files not found! \n";
//        exit(2);
//    }
//
//    std::string dnaA = "", dnaB = "", line;
//    for (int i = 0; std::getline(compare_one, line); i++) {
//        if (i != 0) {
//            dnaA += line;
//        }
//    }
//
//    for (int i = 0; std::getline(compare_two, line); i++) {
//        if (i != 0) {
//            dnaB += line;
//        }
//    }

    // Application of algorithm. // Sequential
    auto *nwseq = new nwseq::NeedlemanWunsch("AATACT", "ATTCT");
    nwseq->calculate_score_matrix();

    std::cout << nwseq->getScoreMatrix()[5][6] << std::endl;

    exit(EXIT_SUCCESS);
}