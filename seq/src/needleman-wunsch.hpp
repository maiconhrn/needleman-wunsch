#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace nwseq {
    static short GAP = -1;

    const static std::vector<std::vector<int>> SIMILARITY_MATRIX = {
            {1,  -1, -1, -1},
            {-1, 1,  -1, -1},
            {-1, -1, 1,  -1},
            {-1, -1, -1, 1}
    };

    const static std::map<char, int> SIMILARITY_MATRIX_CHAR_VALUE = {
            {'A', 0},
            {'T', 1},
            {'C', 2},
            {'G', 3}
    };

    class NeedlemanWunsch {
    private:
        std::string dnaA, dnaB;

        std::vector<std::vector<int>> scoreMatrix;

        void initiateScoreMatrix();

        void populateScoreMatrix();

    public:
        NeedlemanWunsch(std::string dnaA, std::string dnaB);

        void calculate_score_matrix();

        const std::vector<std::vector<int>> &getScoreMatrix() const {
            return scoreMatrix;
        }
    };
}