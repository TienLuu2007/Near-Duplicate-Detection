#ifndef BRUTEFORCEDETECTOR_H
#define BRUTEFORCEDETECTOR_H

#include <vector>
#include <string>
#include <set>
#include <utility>
#include <tuple>
#include <cstdint>

class BruteForceDetector 
{
private:
    int k;  // Shingle size (character-level)
    double threshold;  // Similarity threshold for near-duplicate detection
    
    struct Document 
    {
        std::string id;
        std::string text;  // Preprocessed text
        std::set<std::string> shingles;
    };
    
    std::vector<Document> corpus_docs;   // Reference documents (to compare against)
    std::vector<Document> query_docs;    // Query documents (to check for similarity)
    
    // Helper functions
    std::set<std::string> get_shingles(const std::string& text);
    double jaccard_similarity(const std::set<std::string>& s1, const std::set<std::string>& s2);
    
public:
    // Constructor: k = shingle size, threshold = similarity threshold (0.0 to 1.0)
    BruteForceDetector(int shingle_size = 4, double sim_threshold = 0.7);
    
    // Add a document to the corpus (reference collection)
    void add_corpus_document(const std::string& doc_id, const std::string& preprocessed_text);
    
    // Add a query document (document to be checked against the corpus)
    void add_query_document(const std::string& doc_id, const std::string& preprocessed_text);
    
    // For each query document, find the most similar corpus document
    // Returns: vector of (query_id, corpus_id, similarity_score)
    std::vector<std::tuple<std::string, std::string, double>> find_best_matches();
    
    // For a single query document, find all corpus documents above threshold
    std::vector<std::pair<std::string, double>> find_matches_for_query(const std::string& query_id);
    
    // Print statistics
    void print_statistics() const;
};

#endif