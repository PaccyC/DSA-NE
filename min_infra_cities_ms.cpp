#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <limits>

using namespace std;

struct City {
    int id{};
    string name;
};

struct Road {
    int nbr{};
    string name;
    float budget{};
};

unordered_map<string, City> cities;
unordered_map<string, Road> roads;
unordered_map<string, unordered_set<string>> graph;

const string CITIES_TXT_FILE = "cities.txt";
const string ROADS_TXT_FILE = "roads.txt";

void saveCities() {
    ofstream file(CITIES_TXT_FILE);
    file << "Index    City Name\n";
    for (auto &[id, c] : cities)
        file << c.id << "    " << c.name << "\n";
}

void loadCities() {
    cities.clear();
    ifstream file(CITIES_TXT_FILE);
    string line;
    bool headerSkipped = false;
    while (getline(file, line)) {
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }
        stringstream ss(line);
        string idStr;
        City c;
        getline(ss, idStr, ':');
        getline(ss, c.name, ':');
        c.id = stoi(idStr);
        cities[idStr] = c;
    }
}

void saveRoads() {
    ofstream file(ROADS_TXT_FILE);
    file << "Nbr    Road    Budget\n";
    int count = 1;
    for (auto &[key, r] : roads) {
        file << count++ << "    " << r.name << "    " << r.budget << "\n";
    }
}

void loadRoads() {
    roads.clear();
    graph.clear();
    ifstream file(ROADS_TXT_FILE);
    string line;
    bool headerSkipped = false;
    while (getline(file, line)) {
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }

        stringstream ss(line);
        Road r;
        string budgetStr;
        string nbrStr;
        getline(ss, nbrStr, ':');
        getline(ss, r.name, ':');
        getline(ss, budgetStr, ':');
        r.nbr = stoi(nbrStr);
        r.budget = stof(budgetStr);
        roads[r.name] = r;

        size_t delimiterPos = r.name.find("-");
        if (delimiterPos != string::npos) {
            string city1 = r.name.substr(0, delimiterPos);
            string city2 = r.name.substr(delimiterPos + 1);

            // Restore bidirectional edge in graph
            graph[city1].insert(city2);
            graph[city2].insert(city1);
        }
    }
}

void recordExistingCities() {
    for (int i = 0; i < 7; ++i) {
        City c;
        c.id = i + 1;
        cout << "Enter name for city " << c.id << ": ";
        cin >> c.name;
        cities[to_string(c.id)] = c;
    }
    saveCities();
    cout << "Cities added successfully.\n";
}

void addCity() {
    City c;
    cout << "Enter ID: ";
    cin >> c.id;
    if (cities.count(to_string(c.id))) {
        cout << "City ID already exists.\n";
        return;
    }
    cout << "Enter name for city: ";
    cin >> c.name;
    cities[to_string(c.id)] = c;
    saveCities();
    cout << "City added successfully.\n";
}

bool isValidID(const string &id) {
    return cities.find(id) != cities.end();
}

bool isValidCityName(const string &name) {
    for (const auto &[id, c] : cities) {
        if (c.name == name) return true;
    }
    return false;
}

void editCity() {
    string id;
    cout << "Enter the index of the city to edit: ";
    cin >> id;
    if (!isValidID(id)) {
        cout << "City index not found.\n";
        return;
    }
    City &c = cities[id];
    cout << "Enter new name for city: ";
    cin.ignore();
    getline(cin, c.name);
    saveCities();
    cout << "City updated successfully.\n";
}

void addRoadBetweenCities() {
    string name1, name2;
    cout << "Enter the name of the first city: ";
    cin >> name1;
    cout << "Enter the name of the second city: ";
    cin >> name2;
    if (!isValidCityName(name1) || !isValidCityName(name2)) {
        cout << "One or both city names not found.\n";
        return;
    }
    if (name1 == name2 || graph[name1].count(name2)) {
        cout << "Invalid or existing road.\n";
        return;
    }

    graph[name1].insert(name2);
    graph[name2].insert(name1);

    string roadName = name1 + "-" + name2;
    roads[roadName] = Road{(int)roads.size() + 1, roadName, 0.0f};
    saveRoads();

    cout << "Road added between " << name1 << " and " << name2 << ".\n";
}

void displayRoadsAdjacencyMatrix() {
    if (cities.empty()) {
        cout << "No cities available.\n";
        return;
    }

    vector<pair<int, string>> orderedCities;
    for (auto &[idStr, city] : cities)
        orderedCities.emplace_back(city.id, city.name);

    sort(orderedCities.begin(), orderedCities.end());

    cout << "\nRoads Adjacency Matrix :\n     ";
    for (auto &[id, name] : orderedCities) {
        cout << name << " ";
    }
    cout << "\n";

    for (auto &[idRow, cityRow] : orderedCities) {
        cout << cityRow << " ";
        if (cityRow.length() < 5) cout << string(5 - cityRow.length(), ' ');
        for (auto &[idCol, cityCol] : orderedCities) {
            cout << "    " << (graph[cityRow].count(cityCol) ? "1" : "0");
        }
        cout << "\n";
    }
}

void addBudgetForRoads() {
    string name1, name2;
    cout << "Enter the name of the first city: ";
    cin >> name1;
    cout << "Enter the name of the second city: ";
    cin >> name2;

    if (!isValidCityName(name1) || !isValidCityName(name2)) {
        cout << "One or both city names not found.\n";
        return;
    }

    string roadName1 = name1 + "-" + name2;
    string roadName2 = name2 + "-" + name1;

    if (roads.count(roadName1) == 0 && roads.count(roadName2) == 0) {
        cout << "No road exists between these two cities. Please add the road first.\n";
        return;
    }

    float budget;
    cout << "Enter the budget for the road between " << name1 << " and " << name2 << ": ";
    cin >> budget;
    if (budget <= 0) {
        cout << "Invalid budget. Please enter a positive value.\n";
        return;
    }

    if (roads.count(roadName1))
        roads[roadName1].budget = budget;
    else
        roads[roadName2].budget = budget;

    saveRoads();

    cout << "Budget updated for road between " << name1 << " and " << name2 << ": " << budget << "\n";
}

void displayCities() {
    if (cities.empty()) {
        cout << "No cities found.\n";
        return;
    }
    cout << "Cities:\n";
    for (auto &[id, c] : cities)
        cout << id << ": " << c.name << "\n";
}

void displayRoads() {
    displayCities();
    if (graph.empty()) {
        cout << "No Roads found.\n";
        return;
    }
    displayRoadsAdjacencyMatrix();
}

void searchForCityByItsIndex() {
    string id;
    cout << "Enter city index to search: ";
    cin >> id;
    if (!isValidID(id)) {
        cout << "City not found.\n";
        return;
    }
    cout << "City found: " << cities[id].name << "\n";
}
void displayBudgetsAdjacencyMatrix() {
    if (cities.empty()) {
        cout << "No cities available.\n";
        return;
    }

    vector<pair<int, string>> orderedCities;
    for (auto &[idStr, city] : cities)
        orderedCities.emplace_back(city.id, city.name);

    sort(orderedCities.begin(), orderedCities.end());

    cout << "\nBudgets Adjacency Matrix :\n     ";
    for (auto &[id, name] : orderedCities) {
        cout << name << " ";
    }
    cout << "\n";

    for (auto &[idRow, cityRow] : orderedCities) {
        cout << cityRow << " ";
        if (cityRow.length() < 5) cout << string(5 - cityRow.length(), ' ');
        for (auto &[idCol, cityCol] : orderedCities) {
            string roadName1 = cityRow + "-" + cityCol;
            string roadName2 = cityCol + "-" + cityRow;

            float budget = 0.0;
            if (roads.count(roadName1)) {
                budget = roads[roadName1].budget;
            } else if (roads.count(roadName2)) {
                budget = roads[roadName2].budget;
            }
            cout << " " << setw(5) << budget;
        }
        cout << "\n";
    }
}

void displayRecordedDataOnConsole() {
    displayCities();
    displayRoadsAdjacencyMatrix();
    displayBudgetsAdjacencyMatrix();
}

void menu() {
    cout << "\n===== Ministry of Infrastructure Cities and Roads Management System =====\n";
    cout << "1. Add new city(ies)\n";
    cout << "2. Add roads between Cities\n";
    cout << "3. Add the budget for roads\n";
    cout << "4. Edit City\n";
    cout << "5. Search for a city using its index\n";
    cout << "6. Display Cities\n";
    cout << "7. Display roads\n";
    cout << "8. Display recorded data on console\n";
    cout << "9. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    loadCities();
    loadRoads();
    int choice;
    while (true) {
        menu();
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        switch (choice) {
            case 1:
                if (cities.size() < 7)
                    recordExistingCities();
                else
                    addCity();
                break;
            case 2: addRoadBetweenCities(); break;
            case 3: addBudgetForRoads(); break;
            case 4: editCity(); break;
            case 5: searchForCityByItsIndex(); break;
            case 6: displayCities(); break;
            case 7: displayRoads(); break;
            case 8: displayRecordedDataOnConsole(); break;
            case 9:
                cout << "Exiting... Bye!\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
}
