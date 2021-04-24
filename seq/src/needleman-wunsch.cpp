#include "needleman-wunsch.hpp"

#include <algorithm>
#include <utility>

nwseq::NeedlemanWunsch::NeedlemanWunsch(std::string dnaA,
                                        std::string dnaB)
        : dnaA(std::move(dnaA)), dnaB(std::move(dnaB)) {
    scoreMatrix = std::vector<std::vector<int>>(this->dnaB.length() + 1,
                                                std::vector<int>(this->dnaA.length() + 1));
}

void nwseq::NeedlemanWunsch::initiateScoreMatrix() {
    for (int i = 0, j = 0; j < scoreMatrix[0].size(); ++j) {
        scoreMatrix[i][j] = j * GAP;
    }

    for (int i = 0, j = 0; i < scoreMatrix.size(); ++i) {
        scoreMatrix[i][j] = i * GAP;
    }
}

void nwseq::NeedlemanWunsch::populateScoreMatrix() {
    int match = INT32_MIN, insert = INT32_MIN, del = INT32_MIN,
            ssi, ssj;
    char bi, aj;
    for (int i = 1; i < scoreMatrix.size(); ++i) {
        bi = (char) dnaB[i - 1];
        for (int j = 1; j < scoreMatrix[0].size(); ++j) {
            aj = (char) dnaA[j - 1];
            insert = scoreMatrix[i][j - 1] + GAP;
            ssi = SIMILARITY_MATRIX_CHAR_VALUE.at(bi);
            ssj = SIMILARITY_MATRIX_CHAR_VALUE.at(aj);
            match = scoreMatrix[i - 1][j - 1] + SIMILARITY_MATRIX[ssi][ssj];
            del = scoreMatrix[i - 1][j] + GAP;

            scoreMatrix[i][j] = std::max({insert, match, del});
        }
    }
}

void nwseq::NeedlemanWunsch::calculate_score_matrix() {
    initiateScoreMatrix();
    populateScoreMatrix();
}
