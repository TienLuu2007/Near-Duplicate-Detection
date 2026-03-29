#include "benchmark.h"

metrics runBenchmark(MinHash &minhasher, LSHIndex &lsh_index,
const nlohmann::json &original_library, const nlohmann::json &target_suite)
{
    IndexedData document_features = 
        index_original_documents(const_cast<nlohmann::json&>(original_library), minhasher, lsh_index);

    metrics res;
    res.total_docs_in_library = original_library.size();
    res.total_queries = target_suite.size();

    int query_count = 0;

    std::cout << "\n--- Starting Benchmark Loop ---" << std::endl;

    for (const auto &obj : target_suite)
    {
        query_count++;

        // --- Safe guard ---
        bool has_id = obj.contains("Target_id");
        bool has_truth = obj.contains("Ground_truth");
        bool has_file = obj.contains("Target_file");

        if (!has_id || !has_truth || !has_file) 
        {
            std::cerr << "\nCRITICAL ERROR in target_suite_master.json!" << std::endl;
            std::cerr << "Problem found at entry index: " << query_count << std::endl;
            
            if (!has_id)    std::cerr << " -> Missing key: 'Target_id'" << std::endl;
            if (!has_truth) std::cerr << " -> Missing key: 'Ground_truth'" << std::endl;
            if (!has_file)  std::cerr << " -> Missing key: 'Target_file'" << std::endl;

            std::cerr << "Full Object Content causing the crash:\n" << obj.dump(4) << std::endl;
            
            continue; 
        }

        std::string target_id = obj["Target_id"].get<std::string>();
        std::string ground_truth_id = obj["Ground_truth"].get<std::string>();
        std::string relative_path = obj["Target_file"].get<std::string>();
        
        if (query_count % 10 == 0 || query_count == 1) 
        {
            std::cout << "Benchmarking Query [" << query_count << "/" << res.total_queries << "]: " << target_id << "..." << std::endl;
        }

        std::string full_path = "./" + relative_path;
        std::string extracted_text = load_text_from_file(full_path);

        if (extracted_text.empty()) 
        {
            std::cerr << "Warning: Could not read target file: " << full_path << std::endl;
            continue;
        }
        
        auto start = std::chrono::high_resolution_clock::now();

        std::set<std::string> target_shingles = Shingler::get_shingles(extracted_text, 4);
        std::vector<uint64_t> query_signature = minhasher.compute_signature(target_shingles);

        std::set<std::string> candidates = lsh_index.query_candidates(query_signature);
        
        auto end = std::chrono::high_resolution_clock::now();
        res.total_query_time_ms += std::chrono::duration<double, std::milli>(end - start).count();

        res.total_candidates_considered += candidates.size();

        bool parent_found_in_lsh = false;
        std::vector<std::pair<std::string, double>> ranked_candidates;

        for (const auto& candidate_id : candidates)
        {
            // Safeguard: Check if candidate_id actually exists in indexed features
            if (document_features.shingles.find(candidate_id) == document_features.shingles.end()) 
            {
                continue; 
            }

            double real_jaccard = getJaccard(target_shingles, document_features.shingles[candidate_id]);
            double estimated_jaccard = minhasher.calculate_similarity(query_signature, document_features.signatures[candidate_id]);
            
            res.total_jaccard_error += std::abs(real_jaccard - estimated_jaccard);
            res.total_comparisons++;

            if (real_jaccard >= Config::THRESHOLD) 
            {
                ranked_candidates.push_back({candidate_id, real_jaccard});
                
                if (candidate_id == ground_truth_id) 
                {
                    res.ground_truth_match++;
                    parent_found_in_lsh = true;
                } 
                else 
                {
                    res.collateral_match++;
                }
            } 
            else 
            {
                res.false_positives++;
            }
        }

        if (!parent_found_in_lsh) 
        {
            res.false_negatives++;
        }

        std::sort(ranked_candidates.begin(), ranked_candidates.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });

        for (size_t i = 0; i < ranked_candidates.size(); ++i) 
        {
            if (ranked_candidates[i].first == ground_truth_id) 
            {
                res.mrr_sum += 1.0 / (i + 1);
                break;
            }
        }
    }

    return res;
}