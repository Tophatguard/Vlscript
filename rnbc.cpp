#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <thread>
#include <chrono>
using namespace std;

// Helper function to check if a string is a number
bool isNumber(const string& str) {
    return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
}

// Function to check if a string is an operator (for now it handles basic operators like ==, >, <, etc.)
bool isOperator(const string& str) {
    return str == "==" || str == ">" || str == "<" || str == ">=" || str == "<=" || str == "!=";
}

int main(int argc, char **argv) {
	if (argc != 3) {
		cout << "incorrect usage.\n";
		cout << "correct usage is:\n";
		cout << "rnbc <input file> <output file>.rnb\n";
		return (1);
	}
    ifstream inputFile(argv[1]);
    string word;
	ofstream outputfile(argv[2] + string(".rnb"));

	//all the tokens
    if (inputFile.is_open()) {
		string line;
        while (getline(inputFile, line)) {
			stringstream ssline(line);
			string token = line;
			// Remove leading and trailing spaces
			line.erase(0, line.find_first_not_of(" \t"));
			line.erase(line.find_last_not_of(" \t") + 1);
			
			// Skip empty lines
			if (line.empty()) continue;

            string currentLine = "";

			if (token == "return(0)") {
				outputfile << "exit\n";
				continue;
			} else if (token == "return(1)") {
				outputfile << "echo 'an error has occurred'\n";
				outputfile << "exit\n";
				continue;
			} else if (token == "shush") {
				outputfile << "echo off\n";
				continue;
			}

			if (line.substr(0, 10) == "breakpoint") {
				outputfile << "pause\n";
			}

			// Handle the 'print' statement (with quoted strings)
			if (line.substr(0, 5) == "print") {
				string content = line.substr(5);
				content.erase(0, content.find_first_not_of(" \t"));  // Trim leading spaces
				
				if (content.front() != '\"' && content.back() != '"') {
					content = '%' + content + " %";  // Remove quotes
				}

				if (content.front() == '"' && content.back() == '"') {
					content = content.substr(1, content.length() - 2);  // Remove quotes
				}
				outputfile << "echo " << content << "\n";
				continue;
			}

			if (line.substr(0, 4) == "var ") {
				string content = line.substr(4);
				string whatequal = line.substr(content.length() + 4);
				outputfile << "set " << content << whatequal << "\n";
				continue;
			}

            // If the line starts with "if", process it accordingly
            if (line.substr(0, 2) == "if") {
                // Extract condition (between parentheses)
                size_t startPos = line.find("(") + 1;
                size_t endPos = line.find(")");
                string condition = line.substr(startPos, endPos - startPos);
                string word;

                // Extract action (between curly braces)
                startPos = line.find("{") - 1;
                endPos = line.find("}") + 1;
                string action = line.substr(startPos, endPos - startPos);

                // Trim leading and trailing spaces from condition and action
                condition.erase(0, condition.find_first_not_of(" \t"));
                condition.erase(condition.find_last_not_of(" \t") + 1);

                action.erase(0, action.find_first_not_of(" \t"));
                action.erase(action.find_last_not_of(" \t") + 1);

                // Process the condition: Wrap variables with % (if they are not numbers or operators)
                stringstream ssCondition(condition);
                string tokenInCondition;
                string processedCondition = "";

                while (ssCondition >> tokenInCondition) {
                    // If the token is a variable (alphanumeric) and not a number, wrap it with %
                    if (!isNumber(tokenInCondition) && !isOperator(tokenInCondition)) {
                        processedCondition += "%" + tokenInCondition + " %";
                    } else {
                        // Otherwise, leave the token (numbers and operators) as they are
                        processedCondition += tokenInCondition + " ";
                    }
                }

                // Replace curly braces in the action with parentheses
                replace(action.begin(), action.end(), '{', '(');
                replace(action.begin(), action.end(), '}', ')');

                // Output the processed if statement
                outputfile << "if " << processedCondition << action << "\n";
                continue;  // Skip to the next line after processing
            }
			if (line.substr(0, 3) == "end")  {
				outputfile << ")\n";
			}
			if (line.substr(0, 4) == "func") {
				outputfile << ":" << line.substr(5) << "\n";
			}
			if (line.substr(0, 2) == "()") {
				outputfile << "goto " << line.substr(3) << "\n";
			}
			if (line.substr(0, 7) == "include") {
				if (line.substr(8) == "graphics") {
					outputfile <<  "setlocal EnableDelayedExpansion\nfor /F \"tokens=1,2 delims=#\" %%a in ('\"prompt #$H#$E# & echo on & for %%b in (1) do rem\"') do (\nset \"DEL=%%a\"\n)\n";
				}
			}
			if (line.substr(0, 6) == "enable") {
				if (line.substr(7) == "color") {
					outputfile << ":ColorText\necho off\n<nul set /p \".=%DEL%\" > \"%~2\"\nfindstr /v /a:%1 /R \"^$\" \"%~2\" nul\ndel \"%~2\" > nul 2>&1\nIF %ERRORLEVEL% == 1 goto :bluescreenofdeathuhoh\ngoto :eof\n";
				}
			}
			if (line.substr(0, 6) == "cprint") {
				string color = line.substr(7);
				outputfile << "call :ColorText " << line.substr(7) << "\n";
				outputfile << "echo.\n";
			}
		}
        inputFile.close();
    } else {
		std::cerr << "Fatal error.\n";
        std::cout << "Unable to open file.\n";
		return (1);
    }
};
