#include "needleman-wunsch.hpp"

#include <algorithm>
#include <utility>

NeedlemanWunsch::NeedlemanWunsch(std::string dnaA,
                                 std::string dnaB)
        : dnaA(std::move(dnaA)), dnaB(std::move(dnaB)) {
    SIMILARITY_MATRIX = {
            {1,  -1, -1, -1},
            {-1, 1,  -1, -1},
            {-1, -1, 1,  -1},
            {-1, -1, -1, 1}
    };

    SIMILARITY_MATRIX_CHAR_VALUE = {
            {'A', 0},
            {'T', 1},
            {'C', 2},
            {'G', 3}
    };

    scoreMatrixLinesNum = this->dnaB.length() + 1;
    scoreMatrixColumnsNum = this->dnaA.length() + 1;
    scoreMatrix = std::vector<std::vector<int>>(scoreMatrixLinesNum,
                                                std::vector<int>(scoreMatrixColumnsNum,
                                                                 INT32_MIN));
}

void NeedlemanWunsch::initiateScoreMatrix() {
    for (int i = 0, j = 0; j < scoreMatrixColumnsNum; ++j) {
        scoreMatrix[i][j] = j * GAP;
    }

    for (int i = 0, j = 0; i < scoreMatrixLinesNum; ++i) {
        scoreMatrix[i][j] = i * GAP;
    }
}
