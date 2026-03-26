#include "BruteForceDetector.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <climits>

BruteForceDetector::BruteForceDetector(int shingle_size, double sim_threshold)
    : k(shingle_size), threshold(sim_threshold) {}

std::set<std::string> BruteForceDetector::get_shingles(const std::string& text) 
{
    std::set<std::string> shingles;
    
    if (text.length() < static_cast<size_t>(k)) 
    {
        return shingles;
    }
    
    // Generate overlapping character shingles
    for (size_t i = 0; i <= text.length() - k; ++i) 
    {
        shingles.insert(text.substr(i, k));
    }
    
    return shingles;
}

double BruteForceDetector::jaccard_similarity(const std::set<std::string>& s1, const std::set<std::string>& s2) 
{
    if (s1.empty() || s2.empty()) 
    {
        return 0.0;
    }
    
    // Calculate intersection size
    int intersection = 0;
    for (const auto& shingle : s1) 
    {
        if (s2.find(shingle) != s2.end()) 
        {
            intersection++;
        }
    }
    
    // Calculate union size
    int union_size = s1.size() + s2.size() - intersection;
    
    return static_cast<double>(intersection) / union_size;
}

void BruteForceDetector::add_corpus_document(const std::string& doc_id, const std::string& preprocessed_text) 
{
    Document doc;
    doc.id = doc_id;
    doc.text = preprocessed_text;
    doc.shingles = get_shingles(preprocessed_text);
    
    corpus_docs.push_back(doc);
}

void BruteForceDetector::add_query_document(const std::string& doc_id, const std::string& preprocessed_text) 
{
    Document doc;
    doc.id = doc_id;
    doc.text = preprocessed_text;
    doc.shingles = get_shingles(preprocessed_text);
    
    query_docs.push_back(doc);
}

std::vector<std::tuple<std::string, std::string, double>> BruteForceDetector::find_best_matches() 
{
    std::vector<std::tuple<std::string, std::string, double>> results;
    
    std::cout << "=== Brute-Force Detection ===" << std::endl;
    std::cout << "Corpus: " << corpus_docs.size() << " documents" << std::endl;
    std::cout << "Queries: " << query_docs.size() << " documents" << std::endl;
    std::cout << std::endl;
    
    int matches_found = 0;
    
    // For each query document, compare with all corpus documents
    for (const auto& query : query_docs) 
    {
        double best_similarity = 0.0;
        std::string best_corpus_id = "";
        
        for (const auto& corpus : corpus_docs) 
        {
            double similarity = jaccard_similarity(query.shingles, corpus.shingles);
            
            // Track the best match for this query
            if (similarity > best_similarity) 
            {
                best_similarity = similarity;
                best_corpus_id = corpus.id;
            }
        }
        
        // Only report if similarity meets threshold
        if (best_similarity >= threshold) 
        {
            matches_found++;
            results.push_back({query.id, best_corpus_id, best_similarity});
        }
    }
    
    std::cout << "Matches found: " << matches_found << " / " << query_docs.size() << std::endl;
    
    return results;
}

std::vector<std::pair<std::string, double>> BruteForceDetector::find_matches_for_query(const std::string& query_id) 
{
    std::vector<std::pair<std::string, double>> matches;
    
    // Find the target query document
    const Document* target_query = nullptr;
    for (const auto& query : query_docs) 
    {
        if (query.id == query_id) 
        {
            target_query = &query;
            break;
        }
    }
    
    if (!target_query) 
    {
        std::cerr << "Query document not found: " << query_id << std::endl;
        return matches;
    }
    
    // Compare with all corpus documents
    for (const auto& corpus : corpus_docs) 
    {
        double similarity = jaccard_similarity(target_query->shingles, corpus.shingles);
        
        if (similarity >= threshold) 
        {
            matches.push_back({corpus.id, similarity});
        }
    }
    
    // Sort by similarity descending
    std::sort(matches.begin(), matches.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return matches;
}

void BruteForceDetector::print_statistics() const 
{
    // Corpus documents stats
    int total_corpus_shingles = 0;
    int min_corpus = INT_MAX;
    int max_corpus = 0;
    
    for (const auto& doc : corpus_docs) 
    {
        int size = doc.shingles.size();
        total_corpus_shingles += size;
        min_corpus = std::min(min_corpus, size);
        max_corpus = std::max(max_corpus, size);
    }
    
    // Query documents stats
    int total_query_shingles = 0;
    int min_query = INT_MAX;
    int max_query = 0;
    
    for (const auto& doc : query_docs) 
    {
        int size = doc.shingles.size();
        total_query_shingles += size;
        min_query = std::min(min_query, size);
        max_query = std::max(max_query, size);
    }
}