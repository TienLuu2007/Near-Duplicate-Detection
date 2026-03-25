#include "benchmark.h"

void evaluateQuery(const std::string &detected_id, const std::string &original_id, bool isPlagiarisedCategory, metrics &benchmarkResults)
{
    if (isPlagiarisedCategory) 
    {
        if (detected_id == original_id) 
        {
            benchmarkResults.true_positives++;
        } 
        else 
        {
            benchmarkResults.false_positives++;
        }
    } 
    else 
    {
        if (detected_id == original_id) 
        {
            benchmarkResults.false_negatives++;
        } 
        else 
        {
            benchmarkResults.true_negatives++;
        }
    }
}

metrics benchmark::run(MinHash &minhasher, LSHIndex &lsh_index, const nlohmann::json &dataset)
{
    metrics res;
    for(const auto& [key, item] : dataset.items())
    {
        std::string original_text = item["original"].get<std::string>();
        std::string original_id = key;
        index_document(original_id, original_text, minhasher, lsh_index);
    }

    return res
}