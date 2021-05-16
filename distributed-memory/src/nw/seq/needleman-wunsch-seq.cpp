#include "../needleman-wunsch.hpp"

#include <algorithm>

NeedlemanWunschSeq::NeedlemanWunschSeq(std::string dnaA,
                                       std::string dnaB)
        : NeedlemanWunsch(std::move(dnaA), std::move(dnaB)) {}

void NeedlemanWunschSeq::populateScoreMatrix() {
    int match = INT32_MIN, insert = INT32_MIN, del = INT32_MIN,
            ssi, ssj;
    char bi, aj;

    for (int i = 1; i < scoreMatrixLinesNum; ++i) {
        bi = (char) dnaB[i - 1];

        for (int j = 1; j < scoreMatrixColumnsNum; ++j) {
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

void NeedlemanWunschSeq::calculate_score_matrix() {
    initiateScoreMatrix();
    populateScoreMatrix();
}
