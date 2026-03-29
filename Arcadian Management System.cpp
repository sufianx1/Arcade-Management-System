#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <cctype>
using namespace std;

//         Tree Data Structure to Machine
class Machine{
public:
    int machineID;
    string gameType;
    string status;
    int tokenType;
    Machine* left;
    Machine* right;
    // Constructor to initialize a Machine object
    Machine(int machineID, string game_type, string status, int token_type) {
        this->machineID = machineID;
        this->gameType = game_type;
        this->status = status;
        this->tokenType = token_type;
        left = right = nullptr;
    }
};

// Zone Link List data Structue (Each with a Machine BST) ----------------------
class Zone{
public:
    string zoneName;
    Machine* rootMachine;
    Zone* nextZone;
    // Constructor to initialize a Zone object
    Zone(string zoneName) {
        this->zoneName = zoneName;
        rootMachine = nullptr;
        nextZone = nullptr;
    }
};
// ---------------------- Arcade Management ----------------------
class Arcade{
public:
    Zone* headZone;
    Arcade(): headZone(nullptr){}

    // Insertion of NewZone (insertion at head)
    void addZone(string name){
        Zone* newZone = new Zone(name);
        newZone->nextZone = headZone;
        headZone = newZone;

        // Save zone data to file
        ofstream fout("zones.txt", ios::app);
        fout << name << endl;
        fout.close();
        cout << "Zone '" << name << "' added. " <<endl; 
    }


    // insertion of Machines (Using Tree data Structure)

    Machine* insertMachineBST(Machine* root, int id, string type, string status, int token){
    
        if(root == nullptr){
            return new Machine(id, type, status, token);
        }

        if(id < root->machineID){
            root->left = insertMachineBST(root->left, id, type, status, token);
        }else if(id > root->machineID){
            root->right = insertMachineBST(root->right, id, type, status, token);
        }else{
            cout<<"Machine ID " <<id<< " already exists in this Zone. Skipping Duplicate. "<<endl;
        }
        return root;
    }

    void addMachineToZone(string zoneName, int id, string type, string status, int token){
        Zone* z = headZone;

        while(z && z->zoneName != zoneName){
            z = z->nextZone;
        }

        if (z == nullptr){
            cout<<"Zone '" <<zoneName<< "' not found."<<endl;
            return;
        }

        // Normalize and validate status (case-insensitive)
        {
            string s = status;
            for (char &c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
            if (s == "available") status = "Available";
            else if (s == "in use") status = "In Use";
            else if (s == "reserved") status = "Reserved";
            else if (s == "out of order") status = "Out of Order";
            else {
                cout<<"Invalid status. Please use 'Available', 'In Use', 'Reserved', or 'Out of Order'."<<endl;
                return;
            }
        }
        z->rootMachine = insertMachineBST(z->rootMachine, id, type, status, token);

        
        ofstream fout("machines.txt", ios::app);
        fout<<zoneName<< "," <<id<< "," <<type<< "," <<status<< "," << token <<endl;
        fout.close();
        cout<<"Machine added to Zone '" <<zoneName<< "'."<<endl;
    }


    void displayMachinesInOrder(Machine* root) {
          if(root == nullptr){
            return;
        }
        displayMachinesInOrder(root->left);
        cout << "  ?? Machine ID: " << root->machineID;
        cout<<  " | ?? Game: " << root->gameType;
        cout << " | ?? Status: " << root->status;
        cout<<  " | ?? Token Type: " << root->tokenType << endl;
        displayMachinesInOrder(root->right);
    }

    void displayAllZonesAndMachines(){
        Zone* z = headZone;
        while(z != nullptr){
            cout<<"?? Zone: "<<z->zoneName<<endl;
            displayMachinesInOrder(z->rootMachine);
            z = z->nextZone;
        }
    }

    void countStatusInBST(Machine* root, int& available, int& inUse, int& reserved, int& outOfOrder){
        if(root == nullptr){
            return;
        }
        countStatusInBST(root->left, available, inUse, reserved, outOfOrder);
        if(root->status == "Available"){
            available++;
        }else if(root->status == "In Use"){
            inUse++;
        }else if(root->status == "Reserved"){
            reserved++;
        }else if(root->status == "Out of Order"){
            outOfOrder++;
        }
        countStatusInBST(root->right, available, inUse, reserved, outOfOrder); 
    }

    void showMachineStatusCount(){
        int availableCount = 0, inUseCount = 0, reservedCount = 0, outOfOrderCount = 0;

        for (Zone* z = headZone; z != nullptr; z = z->nextZone) {
            countStatusInBST(z->rootMachine, availableCount, inUseCount, reservedCount, outOfOrderCount);
        }
        cout << "?? Machine Status Count: " << endl;
        cout << "? Available: " << availableCount << endl;
        cout << "?? In Use: " << inUseCount << endl;
        cout << "? Reserved: " << reservedCount << endl;
        cout << "? Out of Order: " << outOfOrderCount << endl;
    }

    // helper to delete machine BST
    void deleteMachineBST(Machine* root) {
        if (root == nullptr) return;
        deleteMachineBST(root->left);
        deleteMachineBST(root->right);
        delete root;
    }

    ~Arcade() {
        Zone* z = headZone;
        while (z != nullptr) {
            Zone* nextZ = z->nextZone;
            deleteMachineBST(z->rootMachine);
            delete z;
            z = nextZ;
        }
    }

    // Machine assigning players
    bool assignPlayerToMachine(string zoneName, string gameType, string playerName){
        Zone* z = headZone;
        while (z != nullptr && z->zoneName != zoneName){
            z = z->nextZone;
        }
        
        if (z == nullptr){
            return false;
        }

        return assignPlayerToMachineBST(z->rootMachine, gameType, playerName);
    }

    bool assignPlayerToMachineBST(Machine* root, string gameType, string playerName) {
        if (root == nullptr){
            return false;
        }
        if(assignPlayerToMachineBST(root->left, gameType, playerName)){
            return true;
        }
        if(root->gameType == gameType && root->status == "Available"){
            root->status = "In Use";
            cout << "? Assigned " << playerName << " to " << root->gameType << " (Machine ID: " << root->machineID << ")." << endl;
            return true;
        }
        return assignPlayerToMachineBST(root->right, gameType, playerName);
    }

};

class TokenRequest{
public:
    string playerName;
    string gameType;
    int sessionTime;
    TokenRequest* next;
    bool highPriority; // true for premium/tournament, false for normal

    TokenRequest(string pname, string gtype, int stime, bool high){
        playerName = pname;
        gameType = gtype;
        sessionTime = stime;
        next = nullptr;
        highPriority = high;
    }
};


class TokenRequestQueue{
public:
    TokenRequest* highFront;
    TokenRequest* highRear;
    TokenRequest* normalFront;
    TokenRequest* normalRear;

    TokenRequestQueue(){
        highFront = highRear = normalFront = normalRear = nullptr;
    }
    ~TokenRequestQueue() {
        TokenRequest* p;
        while (highFront != nullptr) {
            p = highFront;
            highFront = highFront->next;
            delete p;
        }
        while (normalFront != nullptr) {
            p = normalFront;
            normalFront = normalFront->next;
            delete p;
        }
    }    
    void enqueue(string playerName, string gameType, int sessionTime, bool isHighPriority){
        TokenRequest* r = new TokenRequest(playerName, gameType, sessionTime, isHighPriority);
        if (isHighPriority) {
            if(highRear == nullptr){
                highFront = highRear = r;
            }else{
                highRear->next = r;
                highRear = r;
            }
        }else{
            if(normalRear == nullptr){
                normalFront = normalRear = r;
            }else{
                normalRear->next = r;
                normalRear = r;
            }
        }
        ofstream fout("requests.txt", ios::app);
        fout << (isHighPriority ? "High" : "Normal") << "," << playerName << "," << gameType << "," << sessionTime << endl;
        fout.close();
        cout << "Token request added for " << playerName << " (" << (isHighPriority ? "High" : "Normal") << " priority)." << endl;        
    }

    // display queue function
    void displayQueue(){
        cout<<"?? High Priority Queue:"<< endl;
        TokenRequest* p = highFront;
        while(p != nullptr){
            cout<<"  ?? " << p->gameType << " - " << p->sessionTime << " mins - by " << p->playerName << endl;
            p = p->next;
        }
        cout<<"?? Regular Queue:" << endl;
        p = normalFront;
        while (p != nullptr) {
            cout<<"  ?? " << p->gameType << " - " << p->sessionTime << " mins - by " << p->playerName << endl;
            p = p->next;
        }
    }

    TokenRequest* dequeue(){
        if(highFront != nullptr){
            TokenRequest* r = highFront;
            highFront = highFront->next;
            if(highFront == nullptr){
                highRear = nullptr;
            }
            return r;
        }else if(normalFront != nullptr){
            TokenRequest* r = normalFront;
            normalFront = normalFront->next;
            if(normalFront == nullptr){
                normalRear = nullptr;
            }
            return r;
        }
        return nullptr;
    }
};

class SessionRecord{
public:
    string playerName;
    string machineUsed;
    int duration;
    SessionRecord* next;

    SessionRecord(string pname, string mused, int dur){
        playerName = pname;
        machineUsed = mused;
        duration = dur;
        next = nullptr;
    }
};
 
class SessionStack{
public:
    SessionRecord* top;
    SessionStack(){
        top = nullptr;
    }

    void push(string playerName, string machineUsed, int duration){
        SessionRecord* r = new SessionRecord(playerName, machineUsed, duration);
        r->next = top;
        top = r;

        ofstream fout("sessions.txt", ios::app);
        fout << playerName << "," << machineUsed << "," << duration << endl;
        fout.close();
        cout << "Session recorded: " << playerName << " played " << machineUsed
             << " for " << duration << " mins." << endl;
    }
    void pop() {
        if (top != nullptr){
            SessionRecord* temp = top;
            cout<< "?? Session canceled/rolled back: " << temp->playerName << " on " << temp->machineUsed << "." << endl;
            top = top->next;
            delete temp;
        }else{
            cout << "? No session to rollback." << endl;
        }
    }

    void displaySessions() {
        SessionRecord* p = top;
        cout << "?? Session History:" << endl;
        while (p != nullptr) {
            cout<< "  ?? " << p->playerName << " played " << p->machineUsed<< " for " << p->duration << " mins." << endl;
            p = p->next;
        }
    }
};


int main() {
    Arcade arcade;
    TokenRequestQueue tokenQueue;
    SessionStack sessionStack;

    cout << "?? Welcome to the Arcade Management System!" << endl;
    // --------- Add Zones from User Input ---------
    int zoneCount;
    cout << "?? Enter number of zones: ";
    cin >> zoneCount;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < zoneCount; ++i) {
        string zoneName;
        cout << "?? Enter name for zone #" << (i + 1) << ": ";
        getline(cin, zoneName);
        arcade.addZone(zoneName);
    }

    // --------- Add Machines from User Input ---------
    for (Zone* z = arcade.headZone; z != nullptr; z = z->nextZone) {
        int machineCount;
        cout << "?? Enter number of machines for zone '" << z->zoneName << "': ";
        cin >> machineCount;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int j = 0; j < machineCount; j++) {
            int id, tokenType;
            string gameType, status;
            cout << "  ?? Enter machine ID: ";
            cin >> id;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  ?? Enter game type: ";
            getline(cin, gameType);
            cout << "  ?? Enter status (Available/In Use/Reserved/Out of Order): ";
            getline(cin, status);
            cout << "  ?? Enter token type (1/2/3): ";
            cin >> tokenType;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            arcade.addMachineToZone(z->zoneName, id, gameType, status, tokenType);
        }
    }

    int choice;
    while (true) {
        cout << endl << "===== ?? Arcade Menu ?? =====" << endl;
        cout << "1. ?? Show machine status count" << endl;
        cout << "2. ?? Add token request" << endl;
        cout << "3. ?? Display token request queues" << endl;
        cout << "4. ? Batch process token requests" << endl;
        cout << "5. ?? Push completed session (manual)" << endl;
        cout << "6. ?? Cancel last session (rollback)" << endl;
        cout << "7. ?? Display session stack" << endl;
        cout << "8. ?? View saved data files" << endl;
        cout << "0. ?? Exit" << endl;
        cout << "?? Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) {
            break;
        }

        switch (choice) {
        case 1: {
            arcade.showMachineStatusCount();
            break;
        }
        case 2: {
            string playerName, gameType;
            int sessionTime;
            char priority;
            cout << "?? Enter player name: ";
            getline(cin, playerName);
            cout << "?? Enter game type: ";
            getline(cin, gameType);
            cout << "? Enter session time (minutes): ";
            cin >> sessionTime;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "?? Is this a high priority request? (y/n): ";
            cin >> priority;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            tokenQueue.enqueue(playerName, gameType, sessionTime, (priority == 'y' || priority == 'Y'));
            break;
        }
        case 3: {
            tokenQueue.displayQueue();
            break;
        }
        case 4: {
            cout << "?? Before processing: " << endl;
            tokenQueue.displayQueue();
            // Simulate processing: assign to machine and push session
            while (TokenRequest* req = tokenQueue.dequeue()) {
                // Try to assign to any zone
                bool assigned = false;
                for (Zone* z = arcade.headZone; z != nullptr; z = z->nextZone) {
                    if (arcade.assignPlayerToMachine(z->zoneName, req->gameType, req->playerName)) {
                        sessionStack.push(req->playerName, req->gameType, req->sessionTime);
                        assigned = true;
                        break;
                    }
                }
                if (!assigned) {
                    cout << "? No available machine for " << req->playerName << "." << endl;
                }
                delete req;
            }
            cout << "? After processing:" << endl;
            tokenQueue.displayQueue();
            break;
        }
        case 5: {
            string playerName, machineUsed;
            int duration;
            cout << "?? Enter player name: ";
            getline(cin, playerName);
            cout << "?? Enter game type: ";
            getline(cin, machineUsed);
            cout << "? Enter duration (minutes): ";
            cin >> duration;
            cin.ignore();
            sessionStack.push(playerName, machineUsed, duration);
            break;
        }
        case 6: {
            sessionStack.pop();
            break;
        }
        case 7: {
            sessionStack.displaySessions();
            break;
        }
        case 8: {
            cout << endl << "--- Zones ---" << endl;
            ifstream fin1("zones.txt");
            string line;
            while (getline(fin1, line)) {
                cout << line << endl;
            }
            fin1.close();

            cout << endl << "--- Machines ---" << endl;
            ifstream fin2("machines.txt");
            while (getline(fin2, line)) {
                cout << line << endl;
            }
            fin2.close();

            cout << endl << "--- Token Requests ---" << endl;
            ifstream fin3("requests.txt");
            while (getline(fin3, line)) {
                cout << line << endl;
            }
            fin3.close();

            cout << endl << "--- Sessions ---" << endl;
            ifstream fin4("sessions.txt");
            while (getline(fin4, line)) {
                cout << line << endl;
            }
            fin4.close();
            break;
        }
        default: {
            cout << "? Invalid choice. Try again." << endl;
        }
        }
    }
    cout << "?? Exiting program." << endl;
    return 0;
}
