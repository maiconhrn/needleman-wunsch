#include "../needleman-wunsch.hpp"

#include <algorithm>

NeedlemanWunschPar::NeedlemanWunschPar(std::string dnaA,
                                       std::string dnaB)
        : NeedlemanWunsch(std::move(dnaA), std::move(dnaB)) {
    sems = std::vector<std::vector<sem_t *>>(scoreMatrixLinesNum - 1,
                                             std::vector<sem_t *>(4));
    for (int i = 0; i < scoreMatrixLinesNum - 1; ++i) {
        for (int j = 0; j < 4; ++j) {
            sems[i][j] = new sem_t;
            sem_init(sems[i][j], 0, 0);
        }
    }

    sem_post(sems[0][0]);
}

void printMatrix(const std::vector<std::vector<int>> &m) {
    for (int i = 0; i < m.size(); ++i) {
        std::cout << "[";
        for (int j = 0; j < m[0].size(); ++j) {
            std::cout << m[i][j] << ", ";
        }
        std::cout << "\b\b]" << std::endl;
    }
    std::cout << std::endl;
}

void *populateScoreMatrixLineFromTo(void *arg) {
    auto params = (ThreadLineParams *) arg;

    ((NeedlemanWunschPar *) params->ref)->populateScoreMatrixLineFromTo(params);

    return nullptr;
}

void NeedlemanWunschPar::populateScoreMatrixLineFromTo(ThreadLineParams *params) {
    int match = INT32_MIN, insert = INT32_MIN, del = INT32_MIN,
            ssi, ssj;
    char bi, aj;

    for (int i = params->lineNum; i < scoreMatrixLinesNum; ++i) {
        sem_wait(sems[i - 1][params->threadNum]);

        bi = (char) dnaB[i - 1];

        for (int j = params->fromPos; j <= params->toPos; ++j) {
            aj = (char) dnaA[j - 1];
            insert = scoreMatrix[i][j - 1] + GAP;
            ssi = SIMILARITY_MATRIX_CHAR_VALUE.at(bi);
            ssj = SIMILARITY_MATRIX_CHAR_VALUE.at(aj);
            match = scoreMatrix[i - 1][j - 1] + SIMILARITY_MATRIX[ssi][ssj];
            del = scoreMatrix[i - 1][j] + GAP;

            scoreMatrix[i][j] = std::max({insert, match, del});
        }

//        std::cout << "Thread: " << params->threadNum << ", Line: " << i << std::endl;
//        printMatrix(scoreMatrix);

        sem_post(sems[i - 1][params->threadNum]);

        if (params->threadNum != 3) {
            sem_post(sems[i - 1][params->threadNum + 1]);
        }

        if (params->threadNum == 0 && i < sems.size()) {
            sem_post(sems[i][params->threadNum]);
        }
    }
}

void populateScoreMatrixT(NeedlemanWunschPar *ref,
                          std::vector<std::vector<int>> &scoreMatrix) {
    int columnsNum = scoreMatrix[0].size() - 1;
    bool columnSumEven = columnsNum % 2 == 0;
    int quarterLinePos = columnsNum / 4;

    pthread_t thread0, thread1, thread2, thread3;
    auto params0 = ThreadLineParams(0, 1, 1,
                                    quarterLinePos - (columnSumEven ? 0 : 1), ref);
    auto params1 = ThreadLineParams(1, 1, quarterLinePos + (columnSumEven ? 1 : 0),
                                    quarterLinePos * 2 - (columnSumEven ? 0 : 1), ref);
    auto params2 = ThreadLineParams(2, 1, quarterLinePos * 2 + (columnSumEven ? 1 : 0),
                                    quarterLinePos * 3 - (columnSumEven ? 0 : 1), ref);
    auto params3 = ThreadLineParams(3, 1, quarterLinePos * 3 + (columnSumEven ? 1 : 0),
                                    quarterLinePos * 4, ref);

    pthread_create(&thread0,
                   nullptr,
                   populateScoreMatrixLineFromTo,
                   (void *) &params0);
    pthread_create(&thread1,
                   nullptr,
                   populateScoreMatrixLineFromTo,
                   (void *) &params1);
    pthread_create(&thread2,
                   nullptr,
                   populateScoreMatrixLineFromTo,
                   (void *) &params2);
    pthread_create(&thread3,
                   nullptr,
                   populateScoreMatrixLineFromTo,
                   (void *) &params3);

    pthread_join(thread0, nullptr);
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);
    pthread_join(thread3, nullptr);

//    printMatrix(scoreMatrix);
}

void NeedlemanWunschPar::populateScoreMatrix() {
    populateScoreMatrixT(this, this->scoreMatrix);
}

void NeedlemanWunschPar::calculate_score_matrix() {
    initiateScoreMatrix();
    populateScoreMatrix();
}
