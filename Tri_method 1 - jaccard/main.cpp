/* Run:
g++ tokenization.cpp shingling.cpp Hashing.cpp jaccard.cpp main.cpp -o near_dup
./near_dup
*/
#include "function.h"
#include <fstream>

int main()
{
    std::cout << "|-|PROCESSING|-|...\n";
    std::ifstream fin("input.txt");
    std::ofstream fout("output.txt");
    if(fin.is_open() and fout.is_open())
    {
        std::vector<std::string> s;

        std::string line, cell;

       // Read from file, use '\t' to seperate variants.
        while (std::getline(fin, cell, '\t')) 
        {
            if (!cell.empty()) 
            {
                // Some text have 2 or more paragraph, which declare in file dataset as "/text/", so I remove ""
                if (cell.front() == '"' && cell.back() == '"') {
                    cell = cell.substr(1, cell.length() - 2);
                }
                s.push_back(cell);
            }
        }
    
        int n = s.size();
        std::vector<std::vector<std::string>> tokens(n);
        std::vector<std::unordered_set<std::string>> shingles(n);
        std::vector<std::vector<uint32_t>> sig(n);
        
        for (int i = 0; i < n; i++)
        {
            s[i] = normalize(s[i]);
            tokenize(s[i], tokens[i]);
            shingle(tokens[i], k, shingles[i]);
            sig[i] = Hashing(shingles[i]);
            
            if (i >= 1)
            {
                fout << std::fixed << std::setprecision(2) 
                        << Jaccard(sig[0], sig[i]) << "\n";
            }
        }
        fout << "|-|DONE|-|";
    }
    fin.close();
    fout.close();
    std::cout << "|-|DONE|-|";
}