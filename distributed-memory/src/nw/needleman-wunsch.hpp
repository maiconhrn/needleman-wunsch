#ifndef NEEDLEMAN_WUNSCH_H
#define NEEDLEMAN_WUNSCH_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <semaphore.h>
#include <pthread.h>

class NeedlemanWunsch {
public:
    const static short GAP = -1;

    std::vector<std::vector<short>> SIMILARITY_MATRIX;

    std::map<char, short> SIMILARITY_MATRIX_CHAR_VALUE;

    std::string dnaA, dnaB;

    int scoreMatrixLinesNum = INT32_MIN, scoreMatrixColumnsNum = INT32_MIN;

    std::vector<std::vector<int>> scoreMatrix;

    virtual void populateScoreMatrix() = 0;

    NeedlemanWunsch(std::string dnaA, std::string dnaB);

    const std::vector<std::vector<int>> &getScoreMatrix() const {
        return scoreMatrix;
    }

    void initiateScoreMatrix();

    int getResult() {
        return scoreMatrix[scoreMatrixLinesNum - 1][scoreMatrixColumnsNum - 1];
    }

    virtual void calculate_score_matrix() = 0;
};

class NeedlemanWunschSeq : public NeedlemanWunsch {
private:
    void populateScoreMatrix() override;

public:
    NeedlemanWunschSeq(std::string dnaA, std::string dnaB);

    void calculate_score_matrix() override;
};

typedef struct ThreadLineParams {
    int threadNum, fromPos, toPos;

    NeedlemanWunsch *ref;

    ThreadLineParams(int threadNum, int fromPos,
                     int toPos, NeedlemanWunsch *ref);
} ThreadLineParams;

class NeedlemanWunschPar : public NeedlemanWunsch {
private:
    std::vector<std::vector<sem_t *>> sems;

    void populateScoreMatrix() override;

public:
    NeedlemanWunschPar(std::string dnaA, std::string dnaB);

    void populateScoreMatrixLineFromTo(ThreadLineParams *params);

    void calculate_score_matrix() override;
};

#endif //NEEDLEMAN_WUNSCH_H
