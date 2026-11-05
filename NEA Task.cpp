#include <iostream>
#include <ostream>
#include <unordered_map>
#include <string>
#include <random> // IWYU pragma: keep
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

const string SCORES_FILE = "C:/Users/22.o.mozer/OneDrive - The King's School Grantham/School Work/Year 10/Computing/Dice Game/ascores.txt";
const int    NUM_PLAYERS = 2;
const int    NUM_ROUNDS = 5;
const int    MAX_ATTEMPTS = 3;
const int    DICE_MIN = 1;
const int    DICE_MAX = 6;
const int    EVEN_BONUS = 10;
const int    ODD_PENALTY = 5;
const int    TOP_SCORES = 5;
int    i;
string Show_Score;
string Add_User;

struct Player {
    string name;
    int score = 0;
};

bool twoLogin(Player players[]) {
    string username;
    string password;
    int amount_users;

    static unordered_map<string, string> userCreds;
    userCreds["adon"] = "123";
    userCreds["joris"] = "456";
    userCreds["oskar"] = "789";

    cout << "Add a user? Y/N" << "\n";
    cin >> Add_User;

    transform(Add_User.begin(), Add_User.end(), Add_User.begin(), // * Changes "Choice2" to Uppercase 
        ::toupper);

    if (Add_User == "Y") {
        cout << "How many users? ";
        cin >> amount_users;

        for (i; i < amount_users; i++) {
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;;

            if (!userCreds.count(username)) {
                userCreds.insert({ username, password });
            }
            else {
                cout << "Username exists! \n";
            }
        }

    }
    for (int i = 0; i < NUM_PLAYERS; i++) {
        bool granted = false;

        for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
            string username, password;

            cout << "Player " << (i + 1) << " - Enter username: ";
            cin >> username;
            cout << "Player " << (i + 1) << " - Enter password: ";
            cin >> password;

            if (userCreds.find(username) != userCreds.end() && userCreds[username] == password) {
                cout << "User " << (i + 1) << ": Granted" << endl;
                players[i].name = username;
                granted = true;
                break; // stop trying once login succeeds
            }
            else {
                cout << "User " << (i + 1) << ": Denied (attempt " << attempt << " of " << MAX_ATTEMPTS << ")" << endl;
            }
        }

        if (!granted) {
            cout << "\nWARNING: Too many failed login attempts. Closing program." << endl;
            return false;
        }
    }

    return true;
}

void playRound(mt19937& gen, uniform_int_distribution<int>& distrib, Player& player, int roundNum) {
    int die1 = distrib(gen);
    int die2 = distrib(gen);
    int total = die1 + die2;

    cout << "Round " << roundNum << " - " << player.name << " rolled: " << die1 << " and " << die2;

    player.score += total;

    if (total % 2 == 0) {
        player.score += EVEN_BONUS;
        cout << " (even, +" << EVEN_BONUS << ")";
    }
    else {
        player.score -= ODD_PENALTY;
        cout << " (odd, -" << ODD_PENALTY << ")";
    }

    if (die1 == die2) {
        int extra = distrib(gen);
        player.score += extra;
        cout << " (DOUBLE! extra die: " << extra << ")";
    }

    if (player.score < 0) {
        player.score = 0;
    }

    cout << " -> Score so far: " << player.score << endl;
}

void SignUp() {

}

void rollFunc(Player players[]) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distrib(DICE_MIN, DICE_MAX);

    for (int round = 1; round <= NUM_ROUNDS; round++) {
        cout << "\n--- Round " << round << " ---" << endl;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            playRound(gen, distrib, players[i], round);
        }
    }

    cout << endl;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        cout << players[i].name << "'s final score: " << players[i].score << endl;
    }
}

// * Appends the winner's name and score to the external scores file.
void saveWinner(const string& name, int score) {
    ofstream outFile(SCORES_FILE, ios::app);
    if (outFile.is_open()) {
        outFile << name << " " << score << endl; // Writes to file
    }
    else {
        cout << "Warning: could not open " << SCORES_FILE << " to save the result." << endl;
    }
}

void displayTopScores() {

    cout << "Would you like to see high scores? Y/N " << "\n";
    cin >> Show_Score;

    transform(Show_Score.begin(), Show_Score.end(), Show_Score.begin(), // * Changes "Choice" to Uppercase 
        ::toupper);

    if (Show_Score == "Y") {

        ifstream inFile(SCORES_FILE);
        if (!inFile.is_open()) {
            cout << "No past scores found." << endl;
            return;
        }

        vector<pair<string, int>> records;
        string name;
        int score;
        while (inFile >> name >> score) {
            records.push_back(make_pair(name, score));
        }

        if (records.empty()) {
            cout << "No scores recorded yet." << endl;
            return;
        }

        int topCount = min(TOP_SCORES, static_cast<int>(records.size()));
        partial_sort(records.begin(), records.begin() + topCount, records.end(),
            [](const pair<string, int>& a, const pair<string, int>& b) {
                return a.second > b.second;
            });

        cout << "\n--- Top " << TOP_SCORES << " Winning Scores ---" << endl;
        for (int i = 0; i < topCount; i++) {
            cout << (i + 1) << ". " << records[i].first << " - " << records[i].second << endl;
        }
    }
    else if (Show_Score == "N") {
        cout << "Cool, see ya!";
    }
}

int main() {
    Player players[NUM_PLAYERS];

    if (!twoLogin(players)) {
        return 0;
    }

    rollFunc(players);

    if (players[0].score == players[1].score) {
        cout << "\nDraw" << endl;
    }
    else {
        int winner = (players[0].score > players[1].score) ? 0 : 1;
        cout << "\n" << players[winner].name << " wins!" << endl;
        saveWinner(players[winner].name, players[winner].score);
    }

    displayTopScores();

    return 0;
}