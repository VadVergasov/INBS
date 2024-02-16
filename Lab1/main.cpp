#include <string>
#include <iostream>
#include <limits>
#include <cctype>

using namespace std;

void encode_caesar();

void decode_caesar();

void encode_vigener();

void decode_vigener();

void menu() {
    cout << "Choose action (1 - encode with caesar, 2 - decode with caesar, 3 - encode with vigener, 4 - decode with vigener): ";
    size_t number;
    cin >> number;
    switch (number) {
        case 1:
            encode_caesar();
            break;
        case 2:
            decode_caesar();
            break;
        case 3:
            encode_vigener();
            break;
        case 4:
            decode_vigener();
            break;
        case 5:
            break;
        default:
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            menu();
            break;
    }
}

void encode_caesar() {
    string input;
    size_t key;
    cout << "Enter key: ";
    cin >> key;
    while (std::cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << cin.fail() << endl;
        cout << "Incorrect key. Enter key again: ";
        cin >> key;
        cout << cin.fail() << endl;
    }
    cout << "Enter string to encode: ";
    cin >> input;
    for (auto& symbol : input) {
        if (!isalnum(symbol)) {
            cout << "Invalid string!";
            menu();
        }
        symbol += key;
        if (isalpha(symbol - key)) {
            symbol = ((symbol % ((islower(symbol - key) ? 'z' : 'Z') + 1)) + (islower(symbol - key) ? 'a' : 'A')) % (islower(symbol - key) ? 'a' : 'A') + (islower(symbol - key) ? 'a' : 'A');
        } else {
            symbol = ((symbol % ('9' + 1)) + '0') % '0' + '0';
        }
    }
    cout << '\'' << input << '\'' << endl;
}

void decode_caesar() {
    string input;
    size_t key;
    cout << "Enter key: ";
    cin >> key;
    while (std::cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << cin.fail() << endl;
        cout << "Incorrect key. Enter key again: ";
        cin >> key;
        cout << cin.fail() << endl;
    }
    cout << "Enter string to encode: ";
    cin >> input;
    for (auto& symbol : input) {
        if (!isalnum(symbol)) {
            cout << "Invalid string!";
            menu();
        }
        symbol -= key;
        if (isalpha(symbol + key)) {
            if (symbol < (islower(symbol + key) ? 'a' : 'A')) {
                symbol += 26;
            }
        } else {
            if (symbol < '0') {
                symbol += 10;
            }
        }
    }
    cout << '\'' << input << '\'' << endl;
}

void encode_vigener() {
    string key, input;

    cout << "Enter key: ";
    cin >> key;
    cout << "Enter string to encode: ";
    cin >> input;

    while (key.size() < input.size()) {
        key += key;
    }

    string result = "";

    for (int i = 0; i < input.size(); ++i) {
        char plainChar = input[i];
        char keyChar = key[i % key.size()];

        if (isalpha(plainChar)) {
            char base = isupper(plainChar) ? 'A' : 'a';
            char encryptedChar = ((plainChar - base) + (keyChar - base)) % 26 + base;
            result += encryptedChar;
        } else {
            result += plainChar;
        }
    }

    cout << '\'' << result << '\'' << endl;
}

void decode_vigener() {
    string key, input;

    cout << "Enter key: ";
    cin >> key;
    cout << "Enter string to decode: ";
    cin >> input;

    string plaintext;
    int keyLength = key.length();

    for (int i = 0; i < input.size(); ++i) {
        char cipherChar = input[i];
        char keyChar = key[i % keyLength];

        if (isalpha(cipherChar)) {
            char base = isupper(cipherChar) ? 'A' : 'a';
            char decryptedChar = ((cipherChar - base) - (keyChar - base) + 26) % 26 + base;
            plaintext += decryptedChar;
        } else {
            plaintext += cipherChar;
        }
    }

    cout << '\'' << plaintext << '\'' << endl;
}

int main() {
    menu();
    return 0;
}
