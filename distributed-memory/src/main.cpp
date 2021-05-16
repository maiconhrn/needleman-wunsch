#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <chrono>
#include <mpi.h>

#include "nw/needleman-wunsch.hpp"

#define TAG 1

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

std::map<char, short> SIMILARITY_MATRIX_CHAR_VALUE = {
        {'A', 0},
        {'T', 1},
        {'C', 2},
        {'G', 3}
};

std::vector<std::vector<int>> SIMILARITY_MATRIX = {
        {1,  -1, -1, -1},
        {-1, 1,  -1, -1},
        {-1, -1, 1,  -1},
        {-1, -1, -1, 1}
};

void populateScoreMatrixLinesFromTo(std::vector<std::vector<int>> &scoreMatrix,
                                    int scoreMatrixLinesNum,
                                    std::string dnaA,
                                    std::string dnaB,
                                    int fromPos,
                                    int toPos) {
    int match = INT32_MIN, insert = INT32_MIN, del = INT32_MIN, ssi, ssj;
    char bi, aj;

    for (int i = 1; i < scoreMatrixLinesNum; ++i) {
        bi = (char) dnaB[i - 1];

        for (int j = fromPos; j <= toPos; ++j) {
            aj = (char) dnaA[j - 1];
            insert = scoreMatrix[i][j - 1] + NeedlemanWunsch::GAP;
            ssi = SIMILARITY_MATRIX_CHAR_VALUE.at(bi);
            ssj = SIMILARITY_MATRIX_CHAR_VALUE.at(aj);
            match = scoreMatrix[i - 1][j - 1] + SIMILARITY_MATRIX[ssi][ssj];
            del = scoreMatrix[i - 1][j] + NeedlemanWunsch::GAP;

            scoreMatrix[i][j] = std::max({insert, match, del});
        }
    }
}

int main(int argc, char *argv[]) {
    int id, size, count;
    MPI_Status st;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<std::string> args(argv, argv + argc);

    if (id == 0) {
//        std::cout << "id: " << id << " processing files" << std::endl;

        std::ifstream dnaA(args[1]), dnaB(args[2]);
        std::string sdnaA, sdnaB;

        if (!dnaA.fail()) {
            std::getline(dnaA, sdnaA);
        } else {
            std::cerr << "File: " << args[1] << " canot be opened." << std::endl;

            exit(EXIT_FAILURE);
        }

        if (!dnaB.fail()) {
            std::getline(dnaB, sdnaB);
        } else {
            std::cerr << "File: " << args[2] << " canot be opened." << std::endl;

            exit(EXIT_FAILURE);
        }

//        std::cout << id << " dnaA: " << sdnaA.c_str() << std::endl;
//        std::cout << id << " dnaB: " << sdnaB.c_str() << std::endl;

        for (int i = 1; i < size; ++i) {
//            std::cout << "OPA01" << std::endl;
            MPI_Send(sdnaA.c_str(), sdnaA.size(), MPI_CHAR, i, TAG, MPI_COMM_WORLD);
            MPI_Send(sdnaB.c_str(), sdnaB.size(), MPI_CHAR, i, TAG, MPI_COMM_WORLD);
//            std::cout << "OPA02" << std::endl;
        }

        auto nw = new NeedlemanWunschSeq(sdnaA, sdnaB);
        nw->initiateScoreMatrix();
        auto scoreMatrix = nw->getScoreMatrix();

        int processesNum = size - 1;
        int columnsNum = (int) scoreMatrix[0].size() - 1;
        bool columnNumEven = columnsNum % 2 == 0;
        bool processesNumEven = processesNum % 2 == 0;
        int divisionLinePos = columnsNum / processesNum;

        int fromPos = 1, toPos = divisionLinePos;
        MPI_Send(&fromPos, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
        MPI_Send(&toPos, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);

//        std::cout << id << " fromPos: " << fromPos << std::endl;
//        std::cout << id << " toPos: " << toPos << std::endl;

        for (int j = 0; j < scoreMatrix.size(); ++j) {
            MPI_Send(&scoreMatrix[j].front(), scoreMatrix[j].size(),
                     MPI_INT, 1, TAG, MPI_COMM_WORLD);
        }

        for (int i = 0; i < scoreMatrix.size(); ++i) {
            MPI_Probe(processesNum, TAG, MPI_COMM_WORLD, &st);
            MPI_Get_count(&st, MPI_INT, &count);
            MPI_Recv(&scoreMatrix[i].front(), count, MPI_INT, processesNum, TAG, MPI_COMM_WORLD, &st);
        }

        std::cout << scoreMatrix[scoreMatrix.size() - 1][scoreMatrix[0].size() - 1] << std::endl;
    } else {
//        std::cout << "id: " << id << " receiving in strs" << std::endl;

        char cdnaA[100000], cdnaB[100000];

        int source = id - 1;
        int dest = id + 1;

        MPI_Probe(0, TAG, MPI_COMM_WORLD, &st);
        MPI_Get_count(&st, MPI_CHAR, &count);
//        std::cout << id << " dnaA count: " << count << std::endl;
        MPI_Recv(cdnaA, count, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &st);
//        std::cout << id << " dnaA: " << cdnaA << std::endl;

        MPI_Probe(0, TAG, MPI_COMM_WORLD, &st);
        MPI_Get_count(&st, MPI_CHAR, &count);
//        std::cout << id << " dnaB count: " << count << std::endl;
        MPI_Recv(cdnaB, count, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &st);
//        std::cout << id << " dnaB: " << cdnaB << std::endl;

        int fromPos, toPos;
        MPI_Recv(&fromPos, 1, MPI_INT, source, TAG, MPI_COMM_WORLD, &st);
        MPI_Recv(&toPos, 1, MPI_INT, source, TAG, MPI_COMM_WORLD, &st);
//        std::cout << id << " fromPos: " << fromPos << std::endl;
//        std::cout << id << " toPos: " << toPos << std::endl;

        std::string sdnaA(cdnaA), sdnaB(cdnaB);
        auto scoreMatrix = std::vector<std::vector<int>>(sdnaB.length() + 1,
                                                         std::vector<int>(sdnaA.length() + 1,
                                                                          INT32_MIN));
        for (int i = 0; i < scoreMatrix.size(); ++i) {
            MPI_Probe(source, TAG, MPI_COMM_WORLD, &st);
            MPI_Get_count(&st, MPI_INT, &count);
            MPI_Recv(&scoreMatrix[i].front(), count, MPI_INT, source, TAG, MPI_COMM_WORLD, &st);
        }

        populateScoreMatrixLinesFromTo(scoreMatrix, scoreMatrix.size(),
                                       sdnaA, sdnaB, fromPos, toPos);

        int processesNum = size - 1;
//        std::cout << id << " processes num " << processesNum << std::endl;
        int columnsNum = (int) scoreMatrix[0].size() - 1;
        bool columnNumEven = columnsNum % 2 == 0;
        bool processesNumEven = processesNum % 2 == 0;
        int divisionLinePos = columnsNum / processesNum;

        if (id < processesNum) {
            fromPos = (divisionLinePos * id) + 1;
            toPos = (divisionLinePos * (id + 1)) + (((id == processesNum - 1)
                                                     && (!columnNumEven || !processesNumEven))
                                                    ? columnsNum % processesNum
                                                    : 0);
//            std::cout << id << " next fromPos: " << fromPos << std::endl;
//            std::cout << id << " next toPos: " << toPos << std::endl;
            MPI_Send(&fromPos, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD);
            MPI_Send(&toPos, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD);

            for (int j = 0; j < scoreMatrix.size(); ++j) {
                MPI_Send(&scoreMatrix[j].front(), scoreMatrix[j].size(),
                         MPI_INT, dest, TAG, MPI_COMM_WORLD);
            }
        } else {
            for (int j = 0; j < scoreMatrix.size(); ++j) {
                MPI_Send(&scoreMatrix[j].front(), scoreMatrix[j].size(),
                         MPI_INT, 0, TAG, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}