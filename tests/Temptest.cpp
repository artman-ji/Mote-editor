#include <iostream>
#include <string>
#include <random>
#include <cassert>
#include <vector>
#include <utility>
#include "pieceTable.cpp"
int main() {

    std::string a = "A large span of text";
    std::cout << "Example string: " << std::endl;
    std::string idxs;
    std::string outs;
    for (int i = 0; i < a.size(); i++) {
        idxs += std::to_string(i) + " ";
        outs += a[i];
        outs += " ";
    }

    std::cout << idxs << "\n";
    std::cout << outs << "\n\n";
    std::cout << "the piecetable before a delete operation: " << "\n";
    pieceTable p(a);
    p.DisplayTable();
    std::cout << "sequence: \n";
    std::cout << p.DisplayText() << "\n\n"; 

    std::cout << "the piecetable after a delete operation: " << "\n";
    p.Delete(2);
    p.Delete(2);
    p.Delete(2);
    p.Delete(2);
    p.Delete(2);
    p.Delete(2);
    p.DisplayTable();
    std::cout << "sequence: \n";
    std::cout << p.DisplayText() << "\n\n";

    std::cout << "the piecetable after a delete and an insert operation: " << "\n";
    p.Insert('E', 10);
    p.Insert('n', 11);
    p.Insert('g', 12);
    p.Insert('l', 13);
    p.Insert('i', 14);
    p.Insert('s', 15);
    p.Insert('h', 16);
    p.Insert(' ', 17);
    p.Insert('A', 0);
    
    p.DisplayTable();
    std::cout << "sequence: \n";
    std::cout << p.DisplayText() << "\n\n";

    std::cout << "the piecetable after a delete and an insert operation and another delete function: " << "\n";
    p.Delete(10);
    p.Delete(10);
    
    p.DisplayTable();
    std::cout << "sequence: \n";
    std::cout << p.DisplayText() << "\n";


    std::cout << "Starting Differential Fuzz Test..." << std::endl;
    
    std::string initial_text = "Hello, Piece Table!";
    std::string reference = initial_text;
    pieceTable pt(initial_text);
    
    // Setup Random Number Generation
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Distribution for character generation (printable ASCII)
    std::uniform_int_distribution<> char_dist(32, 126);
    
    const int NUM_ITERATIONS = 1000000;
    
    int rightTimes = 0;
    int wrongTimes = 0;
    std::vector<bool> results(NUM_ITERATIONS);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        size_t current_len = reference.length();
        
        // 0 = Insert, 1 = Delete
        std::uniform_int_distribution<> op_dist(0, 1);
        int operation = (current_len == 0) ? 0 : op_dist(gen); 

        if (operation == 0) {
            results[i] = false;
            // --- INSERT ---
            std::uniform_int_distribution<size_t> pos_dist(0, current_len);
            size_t pos = pos_dist(gen);

            // Generate a random string of length 1 to 10
            std::uniform_int_distribution<> len_dist(1, 10);
            int insert_len = len_dist(gen);
            std::string text_to_insert;
            for (int j = 0; j < insert_len; ++j) {
                text_to_insert += static_cast<char>(char_dist(gen));
            }
            
            reference.insert(pos, text_to_insert);
            for (int i = 0; i < insert_len; i++) {
                pt.Insert(text_to_insert[i], pos + i);
            }

        } else {
            results[i] = true;
            // --- DELETE ---
            // Choose a start position between $0$ and $current_len - 1$
            std::uniform_int_distribution<size_t> pos_dist(0, current_len - 1);
            size_t pos = pos_dist(gen);

            // Choose a length to delete between $1$ and the remaining characters
            std::uniform_int_distribution<size_t> len_dist(1, current_len - pos);
            size_t del_len = len_dist(gen);

            reference.erase(pos, del_len);
            for (int i = 0; i < del_len; i++) {
                pt.Delete(pos);
            }
        }

        // --- VERIFY ---
        if (reference != pt.DisplayText()) {
            std::cerr << "Fuzz test Type: " << ((results[i] == 1) ? "Delete" : "Insert") <<std::endl;
            std::cerr << "Fuzz Test Failed at iteration " << i << "!" << std::endl;
            std::cerr << "Reference: " << reference << std::endl;
            std::cerr << "PieceTable: " << pt.DisplayText() << std::endl;
            wrongTimes++;
        }
        else {
            rightTimes++;
        }
    }
    std::cout << "Run Ended with " << NUM_ITERATIONS << " iterations\n";
    std::cout << "right : " << rightTimes << " | wrong : " << wrongTimes << "\n";
}
