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

using namespace std;
using json = nlohmann::json;

void benchmark(MinHash &minhasher, LSHIndex &lsh_index, const json &dataset)
{
    for(const auto& [key, item] : dataset.items())
    {
        string original_text = item["original"];
        string original_id = key;
        index_document(original_id, original_text, minhasher, lsh_index);
    }
    for(const auto& [key, item] : dataset.items())
    {
        for(const auto &variant : item["variants"])
        {
            string variant_id = variant["id"];
            string variant_text = variant["content"];
            detect(minhasher, lsh_index, variant_text, variant_id);
            // cout << "Indexed variant: " << variant_id << endl;
        }
    }
}

int main() {
    MinHash minhasher(100); 
    LSHIndex lsh_index(20, 5);

    string path = "../../Near-Dup dataset/IELTS/writing task 2/text_variants.json";
    json dataset = load_from_dataset(path);
    if (dataset.empty()) 
    {
        cerr << "Dataset is empty or failed to load." << endl;
        return 1;
    }

    benchmark(minhasher, lsh_index, dataset);
    return 0;
}