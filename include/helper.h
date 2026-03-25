#ifndef _HELPER_H
#define _HELPER_H

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <fstream>

#include "preprocessor.h"
#include "shingler.h"
#include "MinHasher.h"
#include "LSH.h"
#include "json.hpp"


std::vector<uint64_t> getSignature(const std::string& text, MinHash& hasher);

void index_document(const std::string& doc_id, const std::string& text, MinHash& hasher, LSHIndex& index);

void detect(MinHash &minhasher, LSHIndex &lsh_index, const std::string &query_text, const std::string &query_id);

nlohmann::json load_from_dataset(const std::string& path);

#endif