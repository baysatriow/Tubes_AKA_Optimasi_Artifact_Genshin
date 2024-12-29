#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <map>
#include <chrono> // Add chrono header

using namespace std;
using namespace std::chrono; // Add chrono namespace

struct Substat {
    string key;
    double value;
};

struct Artifact {
    string setKey;
    string slotKey;
    int rarity;
    int level;
    string mainStatKey;
    vector<Substat> substats;
};

double evaluateCombination(const vector<Artifact> &combination) {
    double score = 0.0;
    for (const auto &artifact : combination) {
        for (const auto &substat : artifact.substats) {
            if (substat.key == "atk") {
                score += substat.value;
            } else if (substat.key == "critRate_") {
                score += substat.value * 2.0;
            } else if (substat.key == "critDMG_") {
                score += substat.value * 1.5;
            }
        }
    }
    return score;
}

void optimizeRecursiveHelper(const vector<vector<Artifact>> &artifactsBySlot,
                             vector<Artifact> &currentCombination,
                             size_t depth, double &bestScore,
                             vector<Artifact> &bestCombination) {
    if (depth == artifactsBySlot.size()) {
        double score = evaluateCombination(currentCombination);
        if (score > bestScore) {
            bestScore = score;
            bestCombination = currentCombination;
        }
        return;
    }

    for (const auto &artifact : artifactsBySlot[depth]) {
        currentCombination.push_back(artifact);
        optimizeRecursiveHelper(artifactsBySlot, currentCombination, depth + 1, bestScore, bestCombination);
        currentCombination.pop_back();
    }
}

vector<Artifact> optimizeRecursive(const vector<vector<Artifact>> &artifactsBySlot) {
    vector<Artifact> bestCombination;
    vector<Artifact> currentCombination;
    double bestScore = -numeric_limits<double>::infinity();
    optimizeRecursiveHelper(artifactsBySlot, currentCombination, 0, bestScore, bestCombination);
    return bestCombination;
}

vector<Substat> parseSubstats(const string &substatStr) {
    vector<Substat> substats;
    stringstream ss(substatStr);
    string pair;
    while (ss >> pair) {
        size_t pos = pair.find(':');
        if (pos != string::npos) {
            Substat substat;
            substat.key = pair.substr(0, pos);
            substat.value = stod(pair.substr(pos + 1));
            substats.push_back(substat);
        }
    }
    return substats;
}

vector<vector<Artifact>> loadArtifacts(const string &filename) {
    ifstream file(filename);
    vector<vector<Artifact>> artifactsBySlot(5);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string slotKey, setKey, mainStatKey, substatStr;
        int rarity, level;
        Artifact artifact;

        ss >> slotKey >> setKey >> rarity >> level >> mainStatKey;
        getline(ss, substatStr);
        artifact.slotKey = slotKey;
        artifact.setKey = setKey;
        artifact.rarity = rarity;
        artifact.level = level;
        artifact.mainStatKey = mainStatKey;
        artifact.substats = parseSubstats(substatStr);

        if (slotKey == "flower") artifactsBySlot[0].push_back(artifact);
        else if (slotKey == "plume") artifactsBySlot[1].push_back(artifact);
        else if (slotKey == "sands") artifactsBySlot[2].push_back(artifact);
        else if (slotKey == "goblet") artifactsBySlot[3].push_back(artifact);
        else if (slotKey == "circlet") artifactsBySlot[4].push_back(artifact);
    }

    return artifactsBySlot;
}

int main() {
    string filename = "../artifacts.txt";
    vector<vector<Artifact>> artifactsBySlot = loadArtifacts(filename);

    if (artifactsBySlot.empty()) {
        cerr << "Error: No artifacts loaded. Exiting.\n";
        return 1;
    }

    string confirm;
    cout << "Start Combinasi Menggunakan Recursive? Y/N : ";
    cin >> confirm;

    if (confirm == "Y" || confirm == "y") {
        // Start timing
        auto start = high_resolution_clock::now();

        auto bestCombination = optimizeRecursive(artifactsBySlot);

        // End timing
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        if (bestCombination.empty()) {
            cerr << "Error: No valid combinations found.\n";
            return 1;
        }

        cout << "Best Combination (Recursive):\n";
        for (const auto &artifact : bestCombination) {
            cout << artifact.slotKey << " from set " << artifact.setKey << " with main stat " << artifact.mainStatKey << "\n";
        }

        // Print execution time
        cout << "\nExecution time (Recursive): " << duration.count() << " milliseconds\n";
    } else {
        cout << "Program Tidak Dijalankan \n";
    }

    return 0;
}
