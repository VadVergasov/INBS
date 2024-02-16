#include <iostream>
#include <des.h>
#include <httplib.h>
#include <common.h>
#include <chrono>

using namespace std;

using namespace httplib;

constexpr const char* server = "server";

int main() {
    Client cli("http://127.0.0.1:8080");

    string username, password;
    cout << "Enter username: ";
    cin >> username;
    while (password.size() != 8) {
        cout << "Enter password (8 symbols): ";
        cin >> password;
    }
    auto response = cli.Get("/start/" + username);

    if (response->status != 200) {
        cout << "Auth server answered with non 200 code: " << response->status << endl;
        return -1;
    }

    string resp = response->body;

    DES c;
    c.setKey(password);

    string result = c.decryptAnyString(resp);

    cout << "First auth server response: " << result << endl;

    string tgs_key = Utils::Split(result, ';').back().substr(0, 8);

    string aut = username + ";" + to_string(chrono::system_clock::to_time_t(chrono::system_clock::now()));

    c.setKey(tgs_key);

    string aut_encrypted = c.encryptAnyString(aut);

    string request_to_tgs = Utils::Split(result, ';').front() + ";" + aut_encrypted + ";" + server;

    response = cli.Post("/grant/" + username, request_to_tgs, "plain/text");

    if (response->status != 200) {
        cout << "Auth server answered with non 200 code: " << response->status << endl;
        return -1;
    }

    resp = c.decryptAnyString(resp);

    cout << "Second auth server response: " << resp << endl;

    Client hello("http://localhost:8081");

    auto tgs = Utils::Split(result, ';').front();

    auto current_time = chrono::system_clock::to_time_t(chrono::system_clock::now());

    aut = username + ";" + to_string(current_time);

    c.setKey(string(Utils::Split(result, ';').back()));

    response = hello.Post("/hello", tgs + ";" + c.encryptAnyString(aut), "plain/text");

    if (response->status != 200) {
        cout << "Server answered with non 200 code: " << response->status << endl;
        return -1;
    }

    cout << "Server responded with: " << c.decryptAnyString(resp) << endl;

    if (std::atoll(c.decryptAnyString(resp).c_str()) != current_time + 1) {
        cout << "Server verification failed!" << endl;
        return 1;
    }
    cout << "OK!" << endl;

    return 0;
}
