#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
using namespace std;

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
		outputfile << "echo off\n";
        while (inputFile >> word) {
			word.erase(0, word.find_first_not_of(" \t"));
            word.erase(word.find_last_not_of(" \t") + 1);
			// Use this if you want to use a breakpoint
			if (word == "breakpoint") {
				outputfile << "set /p t=" << "\n";
			}
			// This returns of the program ran successfully
            if (word == "return(0)") {
				outputfile << "exit\n";
			}
			// This returns with a error
			if (word == "return(1)") {
				outputfile << "echo 'an error has occured'\n";
				outputfile << "exit\n";
			}
			if (word == "include") {
				inputFile >> word;
				// Include <color> is used if you want to use the CPrint command
				if (word == "<color>") {
					outputfile << "setlocal EnableDelayedExpansion\nfor /F \"tokens=1,2 delims=#\" %%a in ('\"prompt #$H#$E# & echo on & for %%b in (1) do rem\"') do (\nset \"DEL=%%a\"\n)\n";
				}
			}

			// Erase any extra spaces before or after a string
            word.erase(0, word.find_first_not_of(" \t"));
            word.erase(word.find_last_not_of(" \t") + 1);

            // Handle "print:" token (special handling for quoted strings)
            if (word == "print") {
                // Read the next word (which should be a quoted string)
                string content;
                inputFile >> ws;  // Skip any whitespace
                getline(inputFile, content);  // Get the full line after 'print:'
				
				if (content.front() != '"' && content.back() != '"') {
					content = "%" + content + "%";
				}
                // Remove leading and trailing spaces from the content
                content.erase(0, content.find_first_not_of(" \t"));
                content.erase(content.find_last_not_of(" \t") + 1);

                // If content starts and ends with quotes, remove them
                if (!content.empty() && content.front() == '"' && content.back() == '"') {
                    content = content.substr(1, content.length() - 2);  // Remove quotes
                }

                // Output the content as print("content")
                outputfile << "echo " << content << "\n";
            }
			// This will create a new line in the compiled script
			if (word == "nl") {
				outputfile << "echo.\n";
			}
			if (word == "cprint") {
				inputFile >> word;
				//this will get the color use windows command "help color" to see what values are what color
				string input = word;
				inputFile >> word;
               
                outputfile << "call :ColorText " << input << " " << word << "\n";
			}
			if (word == "enable") {
				inputFile >> word;
				// use this also to use CPrint
				if (word == "color") {
					outputfile << ":ColorText\necho off\n<nul set /p \".=%DEL%\" > \"%~2\"\nfindstr /v /a:%1 /R \"^$\" \"%~2\" nul\ndel \"%~2\" > nul 2>&1\ngoto :eof\n";
				}
				// Use this if you want a fullscreen ValleyOS program
				if (word == "window_mode_fullscreen") {
					outputfile << "call :ColorText 70 \"ValleyOS     File Edit View Window Help\"\ncall :ColorText 77 \"fhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh\"\necho. \n";
				}
				// Use this if you want a windowed ValleyOS program
				if (word == "window_mode") {
					// Read the next word (which should be a quoted string)
					string content;
					inputFile >> ws;  // Skip any whitespace
					getline(inputFile, content);  // Get the full line after 'window_mode'
				 
					// Remove leading and trailing spaces from the content
					content.erase(0, content.find_first_not_of(" \t"));
					content.erase(content.find_last_not_of(" \t") + 1);
	
					// If content starts and ends with quotes, remove them
					if (!content.empty() && content.front() == '"' && content.back() == '"') {
						content = content.substr(1, content.length() - 2);  // Remove quotes
					}
					outputfile << "call :ColorText 70 \"ValleyOS     File Edit View Window Help\"\ncall :ColorText 77 \"fhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh\"\necho. \n";
					outputfile << "echo.\necho.\necho.\necho.\necho.\necho.\n";
					// set the title
					outputfile << "call :ColorText 70 " << content << "\n";
					outputfile << "call :ColorText 77 sdassadasdasd\n";
					outputfile << "echo.\n";
				}
			}
			// this declares a variable by using: var example = 5 but saying it is equal to something is
			// optional and if you don't tell it to equal anything it will default to 0
			if (word == "var") {
				inputFile >> word;
				outputfile << "set " << word;
				inputFile >> word;
				if (word != "=") {
					outputfile << " = null\n";
				}
				if (word == "=") {
					inputFile >> word;
					outputfile << "=";
					outputfile << word;
					outputfile << "\n";
				}
			}
			// This declares a function
			if (word == "func") {
				inputFile >> word;
				outputfile << ":" << word << "\n";
			}
			// This will goto a function
			if (word == "()") {
				inputFile >> word;
				outputfile << "goto :" << word << "\n";
			}
		}
        inputFile.close();
    } else {
		std::cerr << "Fatal error.\n";
        std::cout << "Unable to open file.\n";
		return (1);
    }
};
