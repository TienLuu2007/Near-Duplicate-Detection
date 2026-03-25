#ifndef _BENCHMARK_H
#define _BENCHMARK_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>

#include "helper.h"
#include "json.hpp"

struct metrics
{
    int true_positives = 0;
    int false_positives = 0;
    int false_negatives = 0;
    int true_negatives = 0;

    // Academic Metrics
    double precision() const { 
        return (true_positives + false_positives == 0) ? 0 : (double)true_positives / (true_positives + false_positives); 
    }
    double recall() const { 
        return (true_positives + false_negatives == 0) ? 0 : (double)true_positives / (true_positives + false_negatives); 
    }
};

class benchmark
{
    private:
        void evaluateQuery(const std::string &detected_id, const std::string &original_id, bool isPlagiarisedCategory, metrics &benchmarkResults);   

        double threshold_ = 0.7;

    public:
        metrics run(MinHash &minhasher, LSHIndex &lsh_index, const nlohmann::json &dataset);

        double getJaccard(const std::string &text1, const std::string &text2);

        void setThreshold(double t) {threshold_ = t;}
};

#endif