#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

struct Bill {
    int value;
    int count;
};

struct InputData {
    vector<Bill> wallet;
    int amount;
    string strategy;
};

struct Result {
    vector<pair<int, int>> dispense;
};



string readFile(const string& filename) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

vector<int> extractNumbers(const string& s) {
    vector<int> nums;
    int n = s.size();

    for (int i = 0; i < n;) {
        if (isdigit(s[i])) {
            int x = 0;
            while (i < n && isdigit(s[i])) {
                x = x * 10 + (s[i] - '0');
                i++;
            }
            nums.push_back(x);
        }
        else {
            i++;
        }
    }

    return nums;
}

string extractStrategy(const string& s) {
    size_t pos = s.find("\"strategy\"");
    if (pos == string::npos) return "";

    pos = s.find("\"", pos + 10);
    pos++;

    size_t end = s.find("\"", pos);

    return s.substr(pos, end - pos);
}

InputData parseInput(const string& text) {
    InputData data;

    vector<int> nums = extractNumbers(text);

    data.amount = nums.back();
    nums.pop_back();

    for (size_t i = 0; i + 1 < nums.size(); i += 2) {
        data.wallet.push_back({ nums[i], nums[i + 1] });
    }

    data.strategy = extractStrategy(text);

    return data;
}



vector<pair<int, int>> bestAnswer;
bool found = false;

void saveAnswer(const vector<int>& used, const vector<Bill>& bills) {
    bestAnswer.clear();

    for (size_t i = 0; i < used.size(); i++) {
        if (used[i] > 0) {
            bestAnswer.push_back({ bills[i].value, used[i] });
        }
    }

    found = true;
}

bool betterMAX(const vector<int>& a, const vector<int>& b,
    const vector<Bill>& bills) {
    for (int i = (int)bills.size() - 1; i >= 0; i--) {
        if (a[i] != b[i]) {
            return a[i] > b[i];
        }
    }
    return false;
}

bool betterMIN(const vector<int>& a, const vector<int>& b,
    const vector<Bill>& bills) {
    for (size_t i = 0; i < bills.size(); i++) {
        if (a[i] != b[i]) {
            return a[i] > b[i];
        }
    }
    return false;
}

int uniformScore(const vector<int>& v) {
    int mn = 1e9;
    int mx = -1e9;

    for (int x : v) {
        mn = min(mn, x);
        mx = max(mx, x);
    }

    return mx - mn;
}

void dfs(
    int idx,
    int remain,
    vector<Bill>& bills,
    vector<int>& current,
    const string& strategy,
    vector<int>& bestVec
) {
    if (remain == 0) {

        if (!found) {
            bestVec = current;
            saveAnswer(current, bills);
            return;
        }

        bool better = false;

        if (strategy == "MAX") {
            better = betterMAX(current, bestVec, bills);
        }
        else if (strategy == "MIN") {
            better = betterMIN(current, bestVec, bills);
        }
        else if (strategy == "UNIFORM") {
            better = uniformScore(current) < uniformScore(bestVec);
        }

        if (better) {
            bestVec = current;
            saveAnswer(current, bills);
        }

        return;
    }

    if (idx == (int)bills.size()) return;

    int val = bills[idx].value;
    int maxTake = min(bills[idx].count, remain / val);

    for (int take = 0; take <= maxTake; take++) {
        current[idx] = take;

        dfs(
            idx + 1,
            remain - take * val,
            bills,
            current,
            strategy,
            bestVec
        );
    }

    current[idx] = 0;
}



void writeOutput(const string& filename,
    const vector<pair<int, int>>& ans) {

    ofstream out(filename);

    out << "[\n{\n";
    out << "\"dispense\": [";

    for (size_t i = 0; i < ans.size(); i++) {
        out
            << "[" << ans[i].first << ", " << ans[i].second << "]";

        if (i + 1 != ans.size()) {
            out << ", ";
        }
    }

    out << "]\n";
    out << "}\n]";
}


int main() {

    string input = readFile("input_dop2.3.json");

    InputData data = parseInput(input);

    sort(data.wallet.begin(), data.wallet.end(),
        [](const Bill& a, const Bill& b) {
            return a.value < b.value;
        });

    vector<int> current(data.wallet.size(), 0);
    vector<int> bestVec(data.wallet.size(), 0);

    dfs(
        0,
        data.amount,
        data.wallet,
        current,
        data.strategy,
        bestVec
    );

    writeOutput("output_dopoln.json", bestAnswer);

    return 0;
}