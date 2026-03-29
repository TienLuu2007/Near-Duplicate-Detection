#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>

#include "Shingler.h"
#include "MinHasher.h"
#include "LSH.h"
#include "Preprocessor.h"
#include "json.hpp"
#include "helper.h"
#include "benchmark.h"

using json = nlohmann::json;

void run_n_report(metrics &benchmarkResults)
{
    std::fstream report_file("benchmark_report.txt", std::ios::out);
    if (!report_file.is_open())
    {
        std::cerr << "Failed to open benchmark report file." << std::endl;
        return;
    }
    report_file << "=== Benchmark Results ===" << std::endl;
    report_file << "--------------------------" << std::endl;
    report_file << "__Config settings: " << std::endl;
    report_file << "K_GRAM: " << Config::K_GRAM << std::endl;
    report_file << "BANDS: " << Config::BANDS << std::endl;
    report_file << "ROWS: " << Config::ROWS << std::endl;
    report_file << "SIGNATURE_SIZE: " << Config::SIGNATURE_SIZE << std::endl;
    report_file << "THRESHOLD: " << Config::THRESHOLD << std::endl;
    report_file << "-------------------------" << std::endl;
    report_file << "Precision at Threshold: " << benchmarkResults.precision_at_threshold() << std::endl;
    report_file << "Recovery Rate: " << benchmarkResults.recovery_rate() << std::endl;
    report_file << "Mean Absolute Error (MAE): " << benchmarkResults.mae() << std::endl;
    report_file << "Average Query Time (ms): " << benchmarkResults.avg_query_time() << std::endl;
    report_file << "Selectivity: " << benchmarkResults.selectivity() << std::endl;
    report_file << "Mean Reciprocal Rank (MRR): " << benchmarkResults.mrr() << std::endl;
    report_file << "=== End of Report ===" << std::endl;

    report_file.close();
}

int main() {
    Config::setConfig(4, 20, 5, 0.7);
    MinHash minhasher; 
    LSHIndex lsh_index;

    std::string project_root = ".";
    std::string original_lib_path = project_root + "/../Near-Dup dataset/Benchmark ready/meta/original_lib.json";
    std::string target_suite_path = project_root + "/../Near-Dup dataset/Benchmark ready/meta/target_suite.json";
    json original_lib = load_from_dataset(original_lib_path);
    json target_suite = load_from_dataset(target_suite_path);
    if (original_lib.empty() || target_suite.empty()) 
    {
        std::cerr << "One or more datasets are empty or failed to load." << std::endl;
        return 1;
    }
    metrics benchmarkResults = runBenchmark(minhasher, lsh_index, original_lib, target_suite, project_root);
    run_n_report(benchmarkResults);
}