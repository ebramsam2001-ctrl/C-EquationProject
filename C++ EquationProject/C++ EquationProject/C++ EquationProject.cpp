#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

// --- Global Data Structures ---
string varNames[100];
int numVars = 0;
float systemMatrix[100][101]; // 0-99 for variables, 100 for RHS constant
int numEq = 0;

// --- Helper Functions ---
int getVarIndex(string name) {
    for (int i = 0; i < numVars; ++i) {
        if (varNames[i] == name) return i;
    }
    varNames[numVars] = name;
    return numVars++;
}

void parseEquation(string eq, int eqIndex) {
    for (int i = 0; i <= 100; ++i) systemMatrix[eqIndex][i] = 0;

    string clean = "";
    for (char c : eq) if (c != ' ') clean += c;

    int eqPos = clean.find('=');
    string lhs = clean.substr(0, eqPos);
    string rhs = clean.substr(eqPos + 1);

    auto processSide = [&](string side, float multiplier) {
        string s = "";
        for (size_t i = 0; i < side.length(); ++i) {
            if (side[i] == '-' && i > 0 && side[i - 1] != '+' && side[i - 1] != '-') s += "+-";
            else s += side[i];
        }

        size_t pos = 0;
        while (pos < s.length()) {
            size_t next = s.find('+', pos);
            if (next == string::npos) next = s.length();
            string term = s.substr(pos, next - pos);

            if (term != "") {
                size_t j = 0;
                string num = "";
                if (term[j] == '+' || term[j] == '-') { num += term[j]; j++; }
                while (j < term.length() && (isdigit(term[j]) || term[j] == '.')) { num += term[j]; j++; }
                string var = term.substr(j);

                float c = 1.0;
                if (num == "" || num == "+") c = 1.0;
                else if (num == "-") c = -1.0;
                else c = stof(num);

                c *= multiplier;

                if (var == "") {
                    systemMatrix[eqIndex][100] -= c;
                }
                else {
                    int vIdx = getVarIndex(var);
                    systemMatrix[eqIndex][vIdx] += c;
                }
            }
            pos = next + 1;
        }
        };

    processSide(lhs, 1.0);
    processSide(rhs, -1.0);
}

void printEquation(float eq[101]) {
    bool first = true;
    for (int i = 0; i < numVars; ++i) {
        if (eq[i] != 0) {
            if (!first && eq[i] > 0) cout << " + ";
            else if (!first && eq[i] < 0) cout << " - ";
            else if (first && eq[i] < 0) cout << "-";

            cout << abs(eq[i]) << varNames[i];
            first = false;
        }
    }
    if (first) cout << "0";
    cout << " = " << eq[100] << "\n";
}

void printMatrix(float mat[100][101], bool includeRHS) {
    for (int i = 0; i < numEq; ++i) {
        for (int j = 0; j < numVars; ++j) {
            cout << mat[i][j] << (j == numVars - 1 ? "" : " ");
        }
        if (includeRHS) cout << " | " << mat[i][100];
        cout << "\n";
    }
}

float getDeterminant(float mat[100][101], int n) {
    float temp[100][101];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) temp[i][j] = mat[i][j];

    float det = 1.0;
    for (int i = 0; i < n; ++i) {
        int pivot = i;
        for (int j = i + 1; j < n; ++j) {
            if (abs(temp[j][i]) > abs(temp[pivot][i])) pivot = j;
        }
        if (pivot != i) {
            for (int k = 0; k < n; ++k) swap(temp[i][k], temp[pivot][k]);
            det *= -1;
        }
        if (temp[i][i] == 0) return 0;

        det *= temp[i][i];
        for (int j = i + 1; j < n; ++j) {
            float factor = temp[j][i] / temp[i][i];
            for (int k = i; k < n; ++k) temp[j][k] -= factor * temp[i][k];
        }
    }
    return det;
}

void solveSystem() {
    float temp[100][101];
    for (int i = 0; i < numEq; ++i)
        for (int j = 0; j <= 100; ++j) temp[i][j] = systemMatrix[i][j];

    for (int i = 0; i < numVars; ++i) {
        int pivot = i;
        for (int j = i + 1; j < numEq; ++j) {
            if (abs(temp[j][i]) > abs(temp[pivot][i])) pivot = j;
        }
        if (abs(temp[pivot][i]) < 1e-5) continue; // Skip if 0

        for (int k = 0; k <= 100; ++k) swap(temp[i][k], temp[pivot][k]);

        float divisor = temp[i][i];
        for (int k = 0; k <= 100; ++k) temp[i][k] /= divisor;

        for (int j = 0; j < numEq; ++j) {
            if (j != i) {
                float factor = temp[j][i];
                for (int k = 0; k <= 100; ++k) temp[j][k] -= factor * temp[i][k];
            }
        }
    }

    bool noSol = false;
    for (int i = 0; i < numEq; ++i) {
        bool allZero = true;
        for (int j = 0; j < numVars; ++j) if (abs(temp[i][j]) > 1e-5) allZero = false;
        if (allZero && abs(temp[i][100]) > 1e-5) noSol = true;
    }

    if (noSol) {
        cout << "No Solution\n";
    }
    else {
        for (int i = 0; i < numVars; ++i) {
            cout << varNames[i] << "=" << temp[i][100] << "\n";
        }
    }
}

int main() {
    string line;
    if (!getline(cin, line)) return 0;
    numEq = stoi(line);

    for (int i = 0; i < numEq; ++i) {
        getline(cin, line);
        parseEquation(line, i);
    }

    string cmd;
    while (cin >> cmd) {
        if (cmd == "num_vars") {
            cout << numVars << "\n";
        }
        else if (cmd == "equation") {
            int i; cin >> i;
            printEquation(systemMatrix[i - 1]);
        }
        else if (cmd == "column") {
            string var; cin >> var;
            int idx = getVarIndex(var);
            for (int i = 0; i < numEq; ++i) cout << systemMatrix[i][idx] << "\n";
        }
        else if (cmd == "add") {
            int i, j; cin >> i >> j;
            float temp[101] = { 0 };
            for (int k = 0; k <= 100; ++k) temp[k] = systemMatrix[i - 1][k] + systemMatrix[j - 1][k];
            printEquation(temp);
        }
        else if (cmd == "subtract") {
            int i, j; cin >> i >> j;
            float temp[101] = { 0 };
            for (int k = 0; k <= 100; ++k) temp[k] = systemMatrix[i - 1][k] - systemMatrix[j - 1][k];
            printEquation(temp);
        }
        else if (cmd == "substitute") {
            string var; int i, j; cin >> var >> i >> j;
            int vIdx = getVarIndex(var);
            float temp[101] = { 0 };
            float coeff = systemMatrix[j - 1][vIdx];

            for (int k = 0; k < numVars; ++k) {
                if (k != vIdx) temp[k] = -systemMatrix[j - 1][k] / coeff;
            }
            temp[100] = systemMatrix[j - 1][100] / coeff;

            float targetCoeff = systemMatrix[i - 1][vIdx];
            float res[101] = { 0 };
            for (int k = 0; k <= 100; ++k) {
                if (k == vIdx) res[k] = 0;
                else if (k == 100) res[k] = systemMatrix[i - 1][k] + targetCoeff * temp[100];
                else res[k] = systemMatrix[i - 1][k] + targetCoeff * temp[k];
            }
            printEquation(res);
        }
        else if (cmd == "D") {
            string peek;
            if (cin.peek() == ' ' || cin.peek() == '\t') {
                cin >> ws;
                int c = cin.peek();
                if (c == '\n' || c == EOF) {
                    printMatrix(systemMatrix, false);
                }
                else {
                    string var; cin >> var;
                    if (var == "value") { // "D_value" equivalent if space typed
                        cout << getDeterminant(systemMatrix, numVars) << "\n";
                    }
                    else {
                        int vIdx = getVarIndex(var);
                        float temp[100][101];
                        for (int r = 0; r < numEq; r++) {
                            for (int c = 0; c < numVars; c++) temp[r][c] = systemMatrix[r][c];
                            temp[r][vIdx] = systemMatrix[r][100];
                        }
                        printMatrix(temp, false);
                    }
                }
            }
            else {
                printMatrix(systemMatrix, false);
            }
        }
        else if (cmd == "D_value") {
            cout << getDeterminant(systemMatrix, numVars) << "\n";
        }
        else if (cmd == "solve") {
            solveSystem();
        }
    }
    return 0;
}