#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

// --- Global Data Structures ---
string varNames[100]; // Variable names
int numVars = 0; // Number of unique variables
float systemMatrix[100][101]; // 0-99 for variables, 100 for RHS constant
int numEq = 0; // Number of equations

// --- Helper Functions ---
int getVarIndex(string name) {
	// Check if variable already exists
    for (int i = 0; i < numVars; ++i) {
        if (varNames[i] == name) return i;
    }
	// Add new variable
    varNames[numVars] = name;
	// Note: This does not handle the case where more than 100 variables are added, but we assume input is well-formed.
	// Increment numVars after assigning the name to ensure correct indexing
    return numVars++;
}

// Parses an equation of the form "2x + 3y = 5" and fills the systemMatrix for the given equation index
void parseEquation(string eq, int eqIndex) {
	// Clear the row for the new equation
    for (int i = 0; i <= 100; ++i) systemMatrix[eqIndex][i] = 0;
	// Remove spaces for easier parsing
    string clean = "";
	// This loop removes all spaces from the input equation string
    for (char c : eq) if (c != ' ') clean += c;
	// Split into LHS and RHS by finding the position of the '=' character
    int eqPos = clean.find('=');
    string lhs = clean.substr(0, eqPos);
    string rhs = clean.substr(eqPos + 1);

	// Helper lambda to process each side of the equation
    auto processSide = [&](string side, float multiplier) {
		// This loop transforms the side of the equation to ensure that all terms are separated by '+' for easier parsing.
		// For example, "2x-3y" becomes "2x+-3y", making it easier to split by '+'.
        string s = "";
		// We iterate through the characters of the side string. If we encounter a '-' that is not at the start and is not preceded by another '+' or '-', we insert a '+' before it to ensure proper separation of terms.
        for (size_t i = 0; i < side.length(); ++i) {
			// If we find a '-' that is not at the beginning of the string and is not preceded by '+' or '-', we insert a '+' before it to ensure that terms are properly separated when we later split by '+'.
            if (side[i] == '-' && i > 0 && side[i - 1] != '+' && side[i - 1] != '-') s += "+-";
            else s += side[i];
        }
        
		// Now we can split the string by '+' to extract individual terms. Each term will be processed to determine its coefficient and variable part.
        size_t pos = 0;
        while (pos < s.length()) {
            size_t next = s.find('+', pos);
			// If no more '+' is found, we take the rest of the string as the last term.
			// string::npos is a constant that represents a non-existent position in the string, so if find returns this value, it means we've reached the end of the string and there are no more '+' characters to split on.
            if (next == string::npos) next = s.length();
			// Extract the term from the current position to the next '+' character
            string term = s.substr(pos, next - pos);

            if (term != "") {
                size_t j = 0;
                string num = "";
				// We parse the coefficient part of the term, which may include a sign ('+' or '-') and digits (possibly with a decimal point). The variable part will be everything that follows the coefficient.
                if (term[j] == '+' || term[j] == '-') { num += term[j]; j++; }
				// This loop continues to append characters to the num string as long as they are digits or a decimal point, effectively extracting the coefficient from the term.
                while (j < term.length() && (isdigit(term[j]) || term[j] == '.')) { num += term[j]; j++; }
                string var = term.substr(j);

				// Convert the coefficient string to a float. If the coefficient is empty or just a sign, we treat it as 1 or -1 accordingly.
                float c = 1.0;
                if (num == "" || num == "+") c = 1.0;
                else if (num == "-") c = -1.0;
				else c = stof(num); // stof converts a string to a float.

				// Apply the multiplier to the coefficient. The multiplier is 1.0 for the left-hand side (LHS) of the equation and -1.0 for the right-hand side (RHS), effectively moving all terms to one side of the equation.
                c *= multiplier;

				// If the variable part is empty, it means this term is a constant and should be added to the RHS (which is stored in systemMatrix[eqIndex][100]). Otherwise, we get the index of the variable and add the coefficient to the corresponding column in the system matrix.
                if (var == "") {
                    systemMatrix[eqIndex][100] -= c;
                }
				// If the variable part is not empty, we get its index and add the coefficient to the system matrix for the current equation index and variable index.
                else {
                    int vIdx = getVarIndex(var);
                    systemMatrix[eqIndex][vIdx] += c;
                }
            }
			pos = next + 1; // Move to the next term
        }
        };

	processSide(lhs, 1.0); // Process the left-hand side (LHS) of the equation with a multiplier of 1.0
	processSide(rhs, -1.0); // Process the right-hand side (RHS) of the equation with a multiplier of -1.0 to move all terms to one side
}

void printEquation(float eq[101]) {
	// This function prints the equation in a human-readable format. It iterates through the coefficients of the variables and constructs a string representation of the equation. It handles signs and formatting to ensure that the output is clear and concise.
    bool first = true;
    for (int i = 0; i < numVars; ++i) {
		// If the coefficient of the variable is not zero, we need to include it in the output. We also handle the formatting for signs and ensure that the first term is printed without a leading '+'.
        if (eq[i] != 0) {
			//  If this is not the first term and the coefficient is positive, we print " + ". If it's not the first term and the coefficient is negative, we print " - ". If it's the first term and the coefficient is negative, we print "-". This ensures that the equation is formatted correctly with appropriate signs.
            if (!first && eq[i] > 0) cout << " + ";
            else if (!first && eq[i] < 0) cout << " - ";
            else if (first && eq[i] < 0) cout << "-";

			// We print the absolute value of the coefficient followed by the variable name. If the coefficient is 1 or -1, we omit the number and just print the variable name.
            cout << abs(eq[i]) << varNames[i];
			// After printing the term, we set first to false to indicate that subsequent terms should include a leading sign if necessary.
            first = false;
        }
    }
	// If no variables were printed (i.e., all coefficients were zero), we print "0" to represent the left-hand side of the equation. Finally, we print the right-hand side of the equation, which is stored in eq[100].
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

// This function calculates the determinant of a square matrix using Gaussian elimination. It first creates a copy of the input matrix to avoid modifying the original. Then, it performs row operations to convert the matrix into an upper triangular form while keeping track of any row swaps (which change the sign of the determinant). Finally, it multiplies the diagonal elements to get the determinant value.
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

// This function solves the system of equations using Gaussian elimination. It first creates a copy of the system matrix to avoid modifying the original. Then, it performs row operations to convert the matrix into reduced row echelon form. After that, it checks for consistency and prints the solutions for the variables if they exist.
void solveSystem() {
	// Create a copy of the system matrix to perform Gaussian elimination without modifying the original matrix.
    float temp[100][101];
	// We copy the system matrix into a temporary matrix called temp. This allows us to perform row operations on temp without affecting the original systemMatrix, which may be needed for other commands or operations later on.
    for (int i = 0; i < numEq; ++i)
        for (int j = 0; j <= 100; ++j) temp[i][j] = systemMatrix[i][j];

	// Perform Gaussian elimination to convert the matrix into reduced row echelon form. This involves selecting pivot elements, normalizing rows, and eliminating variables from other rows.
    for (int i = 0; i < numVars; ++i) {
		// Find the pivot row for the current variable (column i). We look for the row with the largest absolute value in column i to improve numerical stability.
        int pivot = i;
		// This loop iterates through the rows below the current row (i) to find the row with the largest absolute value in column i. This is done to select a good pivot element for Gaussian elimination, which helps to reduce numerical errors and improve the stability of the solution.
        for (int j = i + 1; j < numEq; ++j) {
            if (abs(temp[j][i]) > abs(temp[pivot][i])) pivot = j;
        }
        if (abs(temp[pivot][i]) < 1e-5) continue; // Skip if 0
		
        // If the pivot row is different from the current row, we swap the two rows in the temp matrix. This is necessary to bring the pivot element to the current row for normalization and elimination steps.
        for (int k = 0; k <= 100; ++k) swap(temp[i][k], temp[pivot][k]);

		// Normalize the pivot row by dividing all elements in the row by the pivot element (temp[i][i]). This makes the coefficient of the variable in the pivot row equal to 1, which is essential for the elimination process.
        float divisor = temp[i][i];
        for (int k = 0; k <= 100; ++k) temp[i][k] /= divisor;

		// Eliminate the variable from all other rows by subtracting an appropriate multiple of the pivot row from each of the other rows. This step ensures that the variable corresponding to column i is eliminated from all rows except the pivot row.
        for (int j = 0; j < numEq; ++j) {
            if (j != i) {
                float factor = temp[j][i];
                for (int k = 0; k <= 100; ++k) temp[j][k] -= factor * temp[i][k];
            }
        }
    }

	// After performing Gaussian elimination, we check for consistency of the system. If we find a row where all variable coefficients are zero but the RHS is non-zero, it means there is no solution to the system of equations.
    bool noSol = false;
    for (int i = 0; i < numEq; ++i) {
        bool allZero = true;
        for (int j = 0; j < numVars; ++j) if (abs(temp[i][j]) > 1e-5) allZero = false;
        if (allZero && abs(temp[i][100]) > 1e-5) noSol = true;
    }

	// If there is no solution, we print "No Solution". Otherwise, we print the values of the variables. The values are stored in the last column of the temp matrix after Gaussian elimination.
    if (noSol) {
        cout << "No Solution\n";
    }
	// If there is a solution, we print the values of the variables. The values are stored in the last column of the temp matrix after Gaussian elimination.
    else {
        for (int i = 0; i < numVars; ++i) {
            cout << varNames[i] << "=" << temp[i][100] << "\n";
        }
    }
}


// --- Main Function ---
int main() {
	// Read the number of equations from input. We use getline to read the entire line and then convert it to an integer using stoi. This allows us to handle cases where there may be leading or trailing whitespace in the input.
    string line;
	// We read a line from the standard input and store it in the variable 'line'. If the input fails (e.g., end of file is reached), we return 0 to indicate that the program should terminate.
    if (!getline(cin, line)) return 0;
	// We convert the string 'line' to an integer using the stoi function, which parses the string and returns its integer value. This integer represents the number of equations that will be provided in the input.
    numEq = stoi(line); // line (number of equations) to int value

	// We read each equation from the input and parse it using the parseEquation function. The loop iterates numEq times, reading one equation per iteration. Each equation is expected to be in a specific format (e.g., "2x + 3y = 5"), and the parseEquation function will process the string to fill the systemMatrix with the coefficients of the variables and the constants.
    for (int i = 0; i < numEq; ++i) {
        getline(cin, line);
        parseEquation(line, i);
    }

	// After reading and parsing all the equations, we enter a loop to process commands from the user. The loop continues until there are no more commands to read (e.g., end of file). Each command is read as a string, and based on the command, we perform different operations on the system of equations, such as printing the number of variables, printing specific equations, performing arithmetic operations on equations, calculating determinants, and solving the system.
    string cmd;
    while (cin >> cmd) {
		// We read a command from the standard input and store it in the variable 'cmd'. The program will then check the value of 'cmd' against various possible commands (e.g., "num_vars", "equation", "column", "add", "subtract", "substitute", "D", "D_value", "solve") and execute the corresponding block of code to perform the requested operation on the system of equations.
        if (cmd == "num_vars") {
            cout << numVars << "\n";
        }
		// If the command is "equation", we read an integer 'i' from the input, which represents the index of the equation to print. We then call the printEquation function with the corresponding row from the systemMatrix to display the equation in a human-readable format.
        else if (cmd == "equation") {
            int i; cin >> i;
            printEquation(systemMatrix[i - 1]);
        }
		// If the command is "column", we read a variable name from the input, get its index using the getVarIndex function, and then print the coefficients of that variable for all equations. This allows us to see how a specific variable is represented across all equations in the system.
        else if (cmd == "column") {
            string var; cin >> var;
            int idx = getVarIndex(var);
            for (int i = 0; i < numEq; ++i) cout << systemMatrix[i][idx] << "\n";
        }
		// If the command is "add", we read two integers 'i' and 'j' from the input, which represent the indices of the equations to add together. We create a temporary array to store the result of adding the two equations, and then we call printEquation to display the resulting equation.
        else if (cmd == "add") {
            int i, j; cin >> i >> j;
            float temp[101] = { 0 };
            for (int k = 0; k <= 100; ++k) temp[k] = systemMatrix[i - 1][k] + systemMatrix[j - 1][k];
            printEquation(temp);
        }
		// If the command is "subtract", we read two integers 'i' and 'j' from the input, which represent the indices of the equations to subtract. We create a temporary array to store the result of subtracting equation j from equation i, and then we call printEquation to display the resulting equation.
        else if (cmd == "subtract") {
            int i, j; cin >> i >> j;
            float temp[101] = { 0 };
            for (int k = 0; k <= 100; ++k) temp[k] = systemMatrix[i - 1][k] - systemMatrix[j - 1][k];
            printEquation(temp);
        }
		// If the command is "substitute", we read a variable name and two integers 'i' and 'j' from the input. We then perform substitution by expressing the variable in equation j in terms of the other variables and constants, and then substituting that expression into equation i. The resulting equation is printed using printEquation.
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
		// If the command is "D", we check if there is a space or tab after the command. If there is, we read the next part of the command to determine if we should print the determinant value or the modified matrix for a specific variable. If there is no space or tab, we simply print the system matrix without the RHS constants.
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
		// If the command is "D_value", we calculate and print the determinant of the coefficient matrix (excluding the RHS constants) using the getDeterminant function. This allows us to determine if the system of equations has a unique solution (non-zero determinant) or if it is singular (zero determinant).
        else if (cmd == "D_value") {
            cout << getDeterminant(systemMatrix, numVars) << "\n";
        }
		// If the command is "solve", we call the solveSystem function to solve the system of equations using Gaussian elimination. The function will print the solutions for the variables if they exist, or indicate if there is no solution.
        else if (cmd == "solve") {
            solveSystem();
        }
        // If the command is "q", we break out of the loop and end the program. This allows the user to exit the program gracefully when they are done with their operations.
        else if (cmd == "q") {
            break;
		}
    }
    return 0;
}