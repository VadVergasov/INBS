#include <httplib.h>
#include <des.h>
#include <common.h>
#include <iostream>
#include <chrono>

using namespace httplib;

using namespace std;

constexpr const char* key = "serverkc";

int main() {
    Server svc;

    DES c;
    c.setKey(key);

    svc.Post("/hello", [&](const Request& request, Response& response) {
        auto tgs = Utils::Split(c.decryptAnyString(Utils::Split(request.body, ';').front()), ';');

        string c_ss = tgs.back();
        string client_id = tgs.front();

        cout << "Received tgs from client '" << client_id << "': " << endl;

        for (const auto& cur : tgs) {
            cout << cur << endl;
        }

        c.setKey(c_ss);

        auto aut = Utils::Split(c.decryptAnyString(Utils::Split(request.body, ';').back()), ';');

        cout << "Received aut from client '" << client_id << "': " << endl;

        for (const auto& cur : aut) {
            cout << cur << endl;
        }

        if (client_id != string(aut.front())) {
            response.status = 403;
            const char* resp = "Forbidden (incorrect user)";
            response.set_content(resp, strlen(resp), "text/plain");
            return;
        }

        string answer = "";

        response.status = 200;
        response.body = c.encryptAnyString(to_string(std::atoll(aut.back().c_str()) + 1));
    });

    svc.listen("0.0.0.0", 8081);

    return 0;
}
