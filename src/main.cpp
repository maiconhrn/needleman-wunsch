#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "nw/needleman-wunsch.hpp"

#include <unistd.h>
#include <ctime>

//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0
void process_mem_usage(double &vm_usage, double &resident_set) {
    using std::ios_base;
    using std::ifstream;
    using std::string;

    vm_usage = 0.0;
    resident_set = 0.0;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                >> utime >> stime >> cutime >> cstime >> priority >> nice
                >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}

int main(int argc, char *argv[]) {
    clock_t begin, end;
    double vm, rss;

    std::vector<std::string> args(argv, argv + argc);

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

    auto inParallel = std::find(args.begin(), args.end(), "-p") != args.end();

    if (!inParallel) {
        // Application of algorithm. // Sequential
        begin = clock();

        auto *nwseq = new NeedlemanWunschSeq(sdnaA, sdnaB);
        nwseq->calculate_score_matrix();
        std::cout << "Sequential: " << nwseq->getResult() << std::endl;

        end = clock();

        std::cout << "Time: " << double(end - begin) / CLOCKS_PER_SEC << "s" << std::endl;
        process_mem_usage(vm, rss);
        std::cout << "VM: " << vm << "KB" << std::endl << "RSS: " << rss << "KB" << std::endl;
    } else {
        // Application of algorithm. // Parallel
        begin = clock();

        auto *nwpar = new NeedlemanWunschPar(sdnaA, sdnaB);
        nwpar->calculate_score_matrix();
        std::cout << "Parallel: " << nwpar->getResult() << std::endl;

        end = clock();

        std::cout << "Time: " << double(end - begin) / CLOCKS_PER_SEC << "s" << std::endl;
        process_mem_usage(vm, rss);
        std::cout << "VM: " << vm << "KB" << std::endl << "RSS: " << rss << "KB" << std::endl;
    }

    exit(EXIT_SUCCESS);
}