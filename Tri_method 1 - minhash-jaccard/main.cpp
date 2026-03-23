/* Run:
g++ tokenization.cpp shingling.cpp minhashing.cpp jaccard.cpp main.cpp -o near_dup
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
        /*while (std::getline(fin, line)) 
        {
            std::stringstream lineStream(line);

            // Tách bằng dấu Tab '\t'
            while (std::getline(lineStream, cell, '\t')) 
            {
                // In mỗi ô ra một dòng
                if (!cell.empty()) 
                {
                    //fout << cell << "\n";
                    s.push_back(cell);
                }
            }
        }*/
       // Đọc trực tiếp từ file, dùng '\t' làm điểm dừng
        while (std::getline(fin, cell, '\t')) 
        {
            if (!cell.empty()) 
            {
                // Loại bỏ dấu ngoặc kép dư thừa nếu là Input 2
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
            sig[i] = Minhash(shingles[i]);
            
            // for (uint32_t val : sig[i])
            //     fout << val << " ";
            // fout << "\n";
            
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