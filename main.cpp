#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <fstream>
#include <map>
#include <atomic>

// ================= Constants =================
const int PULL_COST = 300;
const std::unordered_set<int> GUARANTEED_PULLS = {60, 120, 180, 240, 300, 500, 580, 660, 740, 820, 1100, 1200, 1300, 1400, 1500};

// Thread-safe random number generator
thread_local std::mt19937 gen(std::random_device{}() + std::hash<std::thread::id>{}(std::this_thread::get_id()));
std::uniform_real_distribution<> dis(0.0, 1.0);

// Thread-safe progress bar
std::mutex cout_mutex;
void print_progress(int current, int total) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    int bar_width = 50;
    float progress = (float)current / total;
    int pos = bar_width * progress;
    std::cout << "[";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}


// ================= Simulation Logic =================
int simulate_hero_pulls(const std::vector<int>& souls_needed_list) {
    int pulls = 0;
    int pity_counter = 0;
    std::unordered_set<int> used_milestones;
    std::vector<int> souls(souls_needed_list.size(), 0);

    auto add_soul = [&]() {
        for (size_t i = 0; i < souls.size(); ++i) {
            if (souls[i] < souls_needed_list[i]) {
                souls[i]++;
                return;
            }
        }
    };

    while (true) {
        bool all_met = true;
        for (size_t i = 0; i < souls.size(); ++i) {
            if (souls[i] < souls_needed_list[i]) {
                all_met = false;
                break;
            }
        }
        if (all_met) break;

        pulls++;
        pity_counter++;

        if (dis(gen) < 0.02 || pity_counter >= 60) {
            add_soul();
            pity_counter = 0;
        }

        if (GUARANTEED_PULLS.count(pulls) && !used_milestones.count(pulls)) {
            add_soul();
            used_milestones.insert(pulls);
        }
    }
    return pulls;
}

int simulate_fixed_pulls(int num_pulls) {
    int pity_counter = 0;
    std::unordered_set<int> used_milestones;
    int souls = 0;

    for (int pull = 1; pull <= num_pulls; ++pull) {
        pity_counter++;
        if (dis(gen) < 0.02 || pity_counter >= 60) {
            souls++;
            pity_counter = 0;
        }
        if (GUARANTEED_PULLS.count(pull) && !used_milestones.count(pull)) {
            souls++;
            used_milestones.insert(pull);
        }
    }
    return souls;
}


// ================= Statistics & Output =================
void save_results_to_csv(const std::vector<int>& results, const std::string& filename) {
    std::ofstream file(filename);
    file << "result\n";
    for (int res : results) {
        file << res << "\n";
    }
}

std::vector<std::pair<int, int>> get_top_spikes(const std::vector<int>& pulls_results, int num_spikes = 3) {
    if (pulls_results.empty()) return {};

    auto [min_it, max_it] = std::minmax_element(pulls_results.begin(), pulls_results.end());
    int min_val = *min_it;
    int max_val = *max_it;

    int num_bins = 50;
    double bin_width = static_cast<double>(max_val - min_val) / num_bins;
    if (bin_width < 1) bin_width = 1;

    std::map<int, int> counts;
    for (int pull : pulls_results) {
        int bin_index = static_cast<int>((pull - min_val) / bin_width);
        int bin_center = min_val + static_cast<int>(bin_index * bin_width + bin_width / 2);
        counts[bin_center]++;
    }
    
    std::vector<std::pair<int, int>> sorted_counts;
    for(auto const& [pulls, count] : counts) {
        sorted_counts.push_back({pulls, count});
    }
    std::sort(sorted_counts.begin(), sorted_counts.end(), [](const auto& a, const auto& b){
        return a.second > b.second;
    });

    if (sorted_counts.size() > static_cast<size_t>(num_spikes)) {
        sorted_counts.resize(num_spikes);
    }
    
    std::sort(sorted_counts.begin(), sorted_counts.end(), [](const auto& a, const auto& b){
        return a.first < b.first;
    });
    
    return sorted_counts;
}

void print_top_spikes(const std::vector<std::pair<int, int>>& spike_data, int total_souls) {
    std::cout << "--- Top Spikes ---\n";
    std::cout << std::left << std::setw(20) << "Spike" 
              << std::setw(15) << "Pulls" 
              << std::setw(20) << "Diamond Cost" 
              << std::setw(10) << "Souls" << "\n";
    std::cout << std::string(65, '-') << "\n";
    
    std::vector<std::string> labels = {"Most likely", "Second most likely", "Third most likely"};
    for(size_t i = 0; i < spike_data.size(); ++i) {
        std::cout << std::left << std::setw(20) << (i < labels.size() ? labels[i] : "Likely")
                  << std::setw(15) << spike_data[i].first
                  << std::setw(20) << spike_data[i].first * PULL_COST
                  << std::setw(10) << total_souls << "\n";
    }
}

void print_best_worst_case(const std::vector<int>& pulls) {
    if (pulls.empty()) return;
    auto [min_it, max_it] = std::minmax_element(pulls.begin(), pulls.end());
    std::cout << "\nBest Case:\t " << *min_it << " pulls -> " << *min_it * PULL_COST << " Diamonds\n";
    std::cout << "Worst Case:\t " << *max_it << " pulls -> " << *max_it * PULL_COST << " Diamonds\n";
}


// ================= Input Handling =================
int get_valid_input(const std::string& prompt, int default_val, int min_val = 1) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) {
        std::cout << "Using default value: " << default_val << "\n";
        return default_val;
    }
    try {
        int val = std::stoi(line);
        if (val < min_val) {
            std::cout << "Value too small. Using minimum: " << min_val << "\n";
            return min_val;
        }
        return val;
    } catch (const std::exception&) {
        std::cout << "Invalid input. Using default: " << default_val << "\n";
        return default_val;
    }
}


// ================= Simulation Modes =================
void run_target_souls_simulation() {
    int num_simulations = get_valid_input("\nPlease type the Number of Simulations (Default: 100000): ", 100000, 10000);
    
    std::cout << "\nSimulation for 1 or 2 Heroes? Enter 1 or 2: ";
    std::string hero_mode;
    std::getline(std::cin, hero_mode);

    std::vector<int> souls_needed_list;
    int total_souls = 0;

    if (hero_mode == "1") {
        int s1 = get_valid_input("Souls needed for Hero (14=Ascended): ", 14);
        souls_needed_list.push_back(s1);
        total_souls = s1;
    } else if (hero_mode == "2") {
        int s1 = get_valid_input("Souls needed for Hero 1: ", 14);
        int s2 = get_valid_input("Souls needed for Hero 2: ", 14);
        souls_needed_list = {s1, s2};
        total_souls = s1 + s2;
    } else {
        std::cout << "Invalid input. Aborting.\n";
        return;
    }

    std::cout << "\nExecuting " << num_simulations << " simulations...\n";
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<int> results(num_simulations);
    std::vector<std::thread> threads;
    std::atomic<int> progress_counter = 0;
    unsigned int num_threads = std::thread::hardware_concurrency();

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            int start_index = i * (num_simulations / num_threads);
            int end_index = (i + 1) * (num_simulations / num_threads);
            if (i == num_threads - 1) {
                end_index = num_simulations;
            }
            for (int j = start_index; j < end_index; ++j) {
                results[j] = simulate_hero_pulls(souls_needed_list);
                if (i == 0) { // Only one thread updates the progress bar
                    int current_progress = progress_counter.fetch_add(1);
                    if (current_progress % 100 == 0) {
                         print_progress(current_progress, num_simulations);
                    }
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    
    print_progress(num_simulations, num_simulations);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "\nDone in " << std::fixed << std::setprecision(2) << diff.count() << " Seconds.\n\n";

    auto spike_data = get_top_spikes(results);
    print_top_spikes(spike_data, total_souls);
    print_best_worst_case(results);
    
    save_results_to_csv(results, "pull_results.csv");
    std::cout << "\nResults saved to 'pull_results.csv'. Use the Python script to visualize.\n";
}

void run_fixed_pulls_simulation() {
    int scrolls = get_valid_input("How many Destiny Scrolls do you have: ", 0, 0);
    int diamonds = get_valid_input("How many diamonds do you have: ", 0, 0);
    int pulls_possible = scrolls + diamonds / PULL_COST;
    std::cout << "\nYou can do " << pulls_possible << " pulls.\n";

    int num_simulations = get_valid_input("\nNumber of Simulations (Default: 100000): ", 100000, 1000);

    std::cout << "\nExecuting " << num_simulations << " simulations...\n";
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<int> results(num_simulations);
    std::vector<std::thread> threads;
    std::atomic<int> progress_counter = 0;
    unsigned int num_threads = std::thread::hardware_concurrency();

    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            int start_index = i * (num_simulations / num_threads);
            int end_index = (i + 1) * (num_simulations / num_threads);
            if (i == num_threads - 1) {
                end_index = num_simulations;
            }
            for (int j = start_index; j < end_index; ++j) {
                results[j] = simulate_fixed_pulls(pulls_possible);
                if (i == 0) {
                     int current_progress = progress_counter.fetch_add(1);
                    if (current_progress % 100 == 0) {
                         print_progress(current_progress, num_simulations);
                    }
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    
    print_progress(num_simulations, num_simulations);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    double mean_souls = std::accumulate(results.begin(), results.end(), 0.0) / results.size();
    auto [min_it, max_it] = std::minmax_element(results.begin(), results.end());
    
    std::cout << "\n\nResults for Souls obtained in " << pulls_possible << " pulls:\n";
    std::cout << "  Avg Souls: " << std::fixed << std::setprecision(2) << mean_souls << "\n";
    std::cout << "  Min Souls: " << *min_it << "\n";
    std::cout << "  Max Souls: " << *max_it << "\n";

    save_results_to_csv(results, "soul_results.csv");
    std::cout << "\nResults saved to 'soul_results.csv'. Use the Python script to visualize.\n";
    std::cout << "\nSimulation completed in " << diff.count() << " Seconds.\n";
}


// ================= Entry Point =================
int main() {
    std::cout << "Choose Simulation Mode:\n";
    std::cout << "1 - Simulate pulls needed for specific souls (Target Souls Mode)\n";
    std::cout << "2 - Simulate souls gained from fixed pulls (Fixed Pull Mode)\n";
    std::cout << "Enter 1 or 2: ";
    std::string mode;
    std::getline(std::cin, mode);

    if (mode == "1") {
        run_target_souls_simulation();
    } else if (mode == "2") {
        run_fixed_pulls_simulation();
    } else {
        std::cout << "Invalid input. Please enter '1' or '2'.\n";
    }

    return 0;
}