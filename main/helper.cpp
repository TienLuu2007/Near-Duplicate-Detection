#include "helper.h"

std::vector<uint64_t> getSignature(const std::string& text, MinHash& hasher) 
{
    std::string cleaned_text = Preprocessor::clean(text);
    std::set<std::string> shingles = Shingler::get_shingles(cleaned_text, 4);
    std::vector<uint64_t> signature = hasher.compute_signature(shingles);
    return signature;
}

void index_document(const std::string& doc_id, const std::string& text, MinHash& hasher, LSHIndex& index) 
{
    std::vector<uint64_t> signature = getSignature(text, hasher);
    index.add_to_index(doc_id, signature);
    
    // cout << "Indexed (cleaned): " << doc_id << endl;
}
    
void detect(MinHash &minhasher, LSHIndex &lsh_index, const std::string &query_text, const std::string &query_id)
{
    std::fstream log_file("query_log.txt", std::ios::app);
    if (!log_file.is_open()) 
    {
        std::cerr << "Failed to open log file." << std::endl;
        return;
    }  
    std::string cleaned_query = Preprocessor::clean(query_text);
    std::set<std::string> query_shingles = Shingler::get_shingles(cleaned_query, 4);
    std::vector<uint64_t> query_signature = minhasher.compute_signature(query_shingles);
    std::set<std::string> candidates = lsh_index.query_candidates(query_signature);
    
    // Display the results
    if (candidates.empty())
    {
        log_file << "No near-duplicates found for " << query_id << std::endl;
    } 
    else 
    {
        log_file << "Potential near-duplicates found for " << query_id << ":" << std::endl;
        for (const std::string& candidate_id : candidates) 
        {
            log_file << " -> " << candidate_id << std::endl;
        }
    }
    log_file.close();
}

nlohmann::json load_from_dataset(const std::string& path)
{
    std::ifstream file(path);
    if(!file.is_open()) 
    {
        std::cerr << "Failed to open dataset file." << std::endl;
        return nlohmann::json();
    }
    try 
    {
        nlohmann::json dataset;
        file >> dataset;
        return dataset;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return nlohmann::json();
    }
}