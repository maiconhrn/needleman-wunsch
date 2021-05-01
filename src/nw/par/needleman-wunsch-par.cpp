#include "../needleman-wunsch.hpp"

#include <algorithm>

#define THREADS_NUM 16

ThreadLineParams::ThreadLineParams(int threadNum,
                                   int fromPos,
                                   int toPos,
                                   NeedlemanWunsch *ref)
        : threadNum(threadNum),
          fromPos(fromPos),
          toPos(toPos),
          ref(ref) {}

NeedlemanWunschPar::NeedlemanWunschPar(std::string dnaA,
                                       std::string dnaB)
        : NeedlemanWunsch(std::move(dnaA), std::move(dnaB)) {
    sems = std::vector<std::vector<sem_t *>>(scoreMatrixLinesNum - 1,
                                             std::vector<sem_t *>(THREADS_NUM));
    for (int i = 0; i < scoreMatrixLinesNum - 1; ++i) {
        for (int j = 0; j < THREADS_NUM; ++j) {
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

    for (int i = 1; i < scoreMatrixLinesNum; ++i) {
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

        if (params->threadNum != THREADS_NUM - 1) {
            sem_post(sems[i - 1][params->threadNum + 1]);
        }

        if (params->threadNum == 0 && i < sems.size()) {
            sem_post(sems[i][params->threadNum]);
        }
    }
}

void populateScoreMatrixT(NeedlemanWunschPar *ref,
                          std::vector<std::vector<int>> &scoreMatrix) {
    int columnsNum = (int) scoreMatrix[0].size() - 1;
    bool columnNumEven = columnsNum % 2 == 0;
    int divisionLinePos = columnsNum / THREADS_NUM;
    pthread_t threads[THREADS_NUM];
    ThreadLineParams *params[THREADS_NUM];

    for (int i = 0,
                 fromPos = 1,
                 toPos = divisionLinePos - (columnNumEven ? 0 : 1);
         i < THREADS_NUM; ++i) {
        params[i] = new ThreadLineParams(i, fromPos, toPos, ref);
        pthread_create(&threads[i],
                       nullptr,
                       populateScoreMatrixLineFromTo,
                       (void *) params[i]);

        fromPos = (divisionLinePos * (i + 1)) + (columnNumEven ? 1 : 0);
        toPos = divisionLinePos * (i + 2) - (columnNumEven ? 0 : 1);
    }

    for (int i = 0; i < THREADS_NUM; ++i) {
        pthread_join(threads[i], nullptr);
    }

//    printMatrix(scoreMatrix);
}

void NeedlemanWunschPar::populateScoreMatrix() {
    populateScoreMatrixT(this, this->scoreMatrix);
}

void NeedlemanWunschPar::calculate_score_matrix() {
    initiateScoreMatrix();
    populateScoreMatrix();
}
