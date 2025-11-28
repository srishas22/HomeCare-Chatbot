#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream> 
#include <limits> // Required for numeric_limits

using namespace std;

// --- Utility Functions ---

// Utility function to trim leading and trailing whitespace
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// --- Response Template Class ---

template <typename T1, typename T2>
class Response {
private:
    T1 keyword;
    T2 reply;

public:
    Response(T1 k = "", T2 r = "") : keyword(std::move(k)), reply(std::move(r)) {}

    const T1& getKeyword() const { return keyword; }
    const T2& getResponse() const { return reply; }

    void setKeyword(const T1& k) { keyword = k; }
    void setResponse(const T2& r) { reply = r; }
};

// --- Base Class: Chatbot ---

class Chatbot {
protected:
    string name;
    string filename;
    vector<Response<string, string>> responses;

public:    
    static string toLowerCase(const string& str) {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                  [](unsigned char c) { return tolower(c); });
        return lowerStr;
    }

public:
    Chatbot(const string& botName, const string& file) 
        : name(botName), filename(file) {
        loadResponses();
    }

    virtual ~Chatbot() = default;

    virtual void greet() {
        cout << "Hi! Welcome to the HomeCare Services. I'm " << name << ". How can I help you today?" << endl;
    }

    void farewell() const {
        cout << "Goodbye! Have a great day!" << endl;
    }

    void respond(const string& userInput) {
        string lowerInput = toLowerCase(userInput);
        bool found = false;

        for (const auto& r : responses) {
            if (!r.getKeyword().empty() && lowerInput.find(r.getKeyword()) != string::npos) {
                cout << r.getResponse() << endl;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "I don't understand. Can you teach me a response for this?" << endl;
            string keyword, reply;
            cout << "Enter a keyword or phrase: ";
            getline(cin, keyword); 
            cout << "Enter the response for this keyword or phrase: ";
            getline(cin, reply);
            learn(trim(keyword), reply);
        }
    }

    void learn(const string& keyword, const string& reply) {
        string cleanedKeyword = toLowerCase(keyword);
        responses.emplace_back(cleanedKeyword, reply);
        saveResponseToFile(keyword, reply);
        cout << "I've learned a new response! Keyword: '" << cleanedKeyword << "'" << endl;
    }

    void loadResponses() {
        ifstream infile(filename);
        if (!infile) {
            cout << "No previous knowledge found. Starting fresh!" << endl;
            return;
        }

        string keywordLine, reply;
        while (getline(infile, keywordLine) && getline(infile, reply)) {
            string cleanedKeyword = keywordLine;

            size_t endOfSource = keywordLine.find(']');
            if (endOfSource != string::npos) {
                cleanedKeyword = trim(keywordLine.substr(endOfSource + 1));
            } else {
                cleanedKeyword = trim(keywordLine);
            }
            
            if (!cleanedKeyword.empty()) {
                responses.emplace_back(toLowerCase(cleanedKeyword), reply);
            }
        }
        infile.close(); 
    }

    void saveResponseToFile(const string& keyword, const string& reply) {
        ofstream outfile(filename, ios::app);
        if (outfile) {
            outfile << keyword << "\n" << reply << "\n";
            outfile.flush();
        } else {
            cerr << "Error: Unable to save response to file." << endl;
        }
    }
};

// --- Renamed Intermediate Class: HomeCareBaseChatbot ---

class HomeCareBaseChatbot : public Chatbot {
public:
    HomeCareBaseChatbot(const string& botName, const string& file) 
        : Chatbot(botName, file) {}

    void greet() override {
        cout << "Hello! Welcome to the HomeCare Services. "
             << "I'm " << name << ". How can I assist you today?" << endl;
    }
};

// --- Emergency Service Class (Composition) ---

class EmergencyService {
public:
    void provideEmergencyInfo() const {
        cout << "In case of urgency, please contact "
             << "+91 123 4567890 for immediate assistance." << endl;
    }
};

// --- Final Derived Class: HomeCareChatbot ---

class HomeCareChatbot : public HomeCareBaseChatbot {
private:
    EmergencyService emergencyService;
    
    static const vector<string> PRIMARY_MENU;
    static const vector<string> SECONDARY_MENU;

public:
    HomeCareChatbot(const string& botName, const string& file) 
        : HomeCareBaseChatbot(botName, file) {}

    void provideEmergencyService() const {
        emergencyService.provideEmergencyInfo();
    }

    void displayMenu(bool isPrimary = true) const {
        const vector<string>& menu = isPrimary ? PRIMARY_MENU : SECONDARY_MENU;
        const string title = isPrimary ? "🏡 Primary Options" : "🧹 Service Options";

        cout << "\n--- " << title << " ---" << endl;
        
        for (size_t i = 0; i < menu.size(); ++i) {
            cout << i + 1 << ". " << menu[i] << endl;
        }
        
        if (isPrimary) {
            cout << menu.size() + 1 << ". Emergency Contact" << endl;
        }

        cout << "--------------------------" << endl;
        cout << "Type the option number, or type your query directly (e.g., timings, hi, bye)." << endl;
    }

    const vector<string>& getPrimaryKeywords() const {
        return PRIMARY_MENU;
    }
    
    const vector<string>& getSecondaryKeywords() const {
        return SECONDARY_MENU;
    }

    // Function to handle menu selection and command processing
    bool handleMenuSelection(int menuChoice) {
        
        const vector<string>& primary = getPrimaryKeywords();
        const vector<string>& secondary = getSecondaryKeywords();
        
        string keywordToSearch;
        
        // --- 1. Check Primary Menu ---
        if (menuChoice > 0 && menuChoice <= (int)primary.size()) {
            keywordToSearch = primary[menuChoice - 1];
        } else if (menuChoice == (int)primary.size() + 1) {
            // Emergency contact
            provideEmergencyService();
            return true;
        } 
        // --- 2. Check Secondary Menu ---
        else if (menuChoice > 100 && menuChoice <= 100 + (int)secondary.size()) {
            // Prefix 100 is applied in main(). Index is (choice - 101).
            keywordToSearch = secondary[menuChoice - 101]; 
        } 
        else {
            cout << "Invalid menu choice. Please try again." << endl;
            return false;
        }
        
        // --- 3. Process Keyword ---
        
        if (keywordToSearch == "services") {
            cout << "Great! Which service are you interested in?" << endl;
            displayMenu(false); // Display secondary menu
            return true;
        } else {
            // FIX: Use the base class's respond logic for all specific keywords
            Chatbot::respond(keywordToSearch); 
            return true;
        }
    }
};

// Initialize the static constant members
const vector<string> HomeCareChatbot::PRIMARY_MENU = {
    "services",      
    "appointment",   
    "pricing",       
    "location",      
    "hours",         
    "feedback"       
};

const vector<string> HomeCareChatbot::SECONDARY_MENU = {
    "deep cleaning",      
    "kitchen cleaning",   
    "bathroom cleaning",  
    "carpet cleaning",    
    "dusting",            
    "floor cleaning"      
};


// --- Main Program ---

int main() {
    HomeCareChatbot myBot("Service Bot", "details.txt");

    myBot.greet();
    string userInput;
    int menuChoice = -1; 
    
    bool inSecondaryMenu = false;

    myBot.displayMenu(); 

    while (true) {
        cout << "You: ";
        
        // --- Handle Numeric/Menu Input ---
        if (cin >> menuChoice) {
            
            // 1. Determine the number to pass to handleMenuSelection
            if (inSecondaryMenu) {
                menuChoice += 100; // Apply prefix
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer
            
            // 2. Process the selection
            bool success = myBot.handleMenuSelection(menuChoice);
            
            if (success) {
                // If the successful action was to select 'services' (Primary Choice 1)
                // This correctly handles the transition to the secondary menu
                if (menuChoice == 1) {
                    inSecondaryMenu = true; 
                } else {
                    // For ALL other valid primary/secondary choices, reset state
                    inSecondaryMenu = false; 
                }
            } else {
                // Invalid choice in handleMenuSelection
                inSecondaryMenu = false; // Always reset to primary on invalid input
            }

        } 
        // --- Handle Text Input ---
        else {
            cin.clear();
            getline(cin, userInput);
            
            string lowerInput = Chatbot::toLowerCase(userInput);

            if (lowerInput == "bye" || lowerInput == "exit") {
                myBot.farewell();
                break;
            } else if (lowerInput == "emergency") {
                myBot.provideEmergencyService();
            } else {
                myBot.respond(userInput);
            }
            
            inSecondaryMenu = false; // Reset state after any general text input
        }
        
        // Display the correct menu for the next turn
        myBot.displayMenu(!inSecondaryMenu);
    }

    return 0;
}