#include "des.h"
#include <iostream>
#include <sstream>

using namespace std;

bitset<32> DES::F(bitset<32> R, bitset<48> k) {
    // Расширение электронного блока
    bitset<48> expandR;
    for (int i = 0; i < 48; i++)
        expandR[47 - i] = R[32 - E[i]]; //expandR[i] = R[E[i] - 1];
                                        // XOR
    expandR = expandR ^ k;
    // вместо этого S-поле
    bitset<32> output;
    int x = 0;
    for (int i = 0; i < 48; i = i + 6) {
        int row = expandR[i] * 2 + expandR[i + 5];
        int col = expandR[i + 1] * 8 + expandR[i + 2] * 4 + expandR[i + 3] * 2 + expandR[i + 4];
        int num = S_BOX[i / 6][row][col];
        bitset<4> temp(num);
        output[x + 3] = temp[0];
        output[x + 2] = temp[1];
        output[x + 1] = temp[2];
        output[x] = temp[3];
        x += 4;
    }
    // Замена P-бокса
    bitset<32> tmp = output;
    for (int i = 0; i < 32; i++)
        output[i] = tmp[P[i] - 1];

    return output;
}
// Функция сдвига влево
bitset<28> DES::leftShift(const bitset<28>& bits, int rotateCount) {
    return (bits << rotateCount) | (bits >> (bits.size() - rotateCount));
}

void DES::generateKeys() {
    bitset<56> real_key;
    bitset<28> left;
    bitset<28> right;
    bitset<48> compressKey;

    // Во-первых, после выбора и замены ПК-1 удаляем 8-битный бит четности исходного ключа
    // И переставляем
    for (int i = 0; i < 56; i++)
        real_key[i] = key[PC_1[i] - 1];

    for (int round = 0; round < 16; round++) {
        for (int i = 0; i < 28; i++)
            left[i] = real_key[i];
        for (int i = 28; i < 56; i++)
            right[i - 28] = real_key[i];
        // Сдвиг влево
        left = leftShift(left, shiftBits[round]);
        right = leftShift(right, shiftBits[round]);
        // Подключаем, заменяем и выбираем ПК-2 для перестановки и сжатия
        for (int i = 0; i < 28; i++)
            real_key[i] = left[i];
        for (int i = 28; i < 56; i++)
            real_key[i] = right[i - 28];
        for (int i = 0; i < 48; i++) {
            int m = PC_2[i];
            compressKey[i] = real_key[m - 1];
        }

        subkey[round] = compressKey;
    }
}

// Функция инструмента: выполнить двоичное обратное преобразование
bitset<64> DES::change(bitset<64> temp) {
    bitset<64> bits;
    bitset<8> n;
    for (int i = 0; i < 64; i = i + 8) {
        for (int j = 0; j < 8; j++) {
            bits[i + j] = temp[i + 7 - j];
        }
    }
    return bits;
}

bitset<64> DES::DES_encrypt(bitset<64>& plain) {
    bitset<64> cipher;
    bitset<64> currentBits;
    bitset<32> left;
    bitset<32> right;
    bitset<32> newLeft;
    // Первоначальная замена IP
    for (int i = 0; i < 64; i++)
        currentBits[i] = plain[IP[i] - 1]; //

    for (int i = 0; i < 32; i++)
        left[i] = currentBits[i];
    for (int i = 32; i < 64; i++)
        right[i - 32] = currentBits[i];
    // Вводим 16 раундов изменения
    for (int round = 0; round < 16; round++) {
        newLeft = right;
        right = left ^ F(right, subkey[round]);
        left = newLeft;
    }
    // Слияние
    for (int i = 0; i < 32; i++)
        cipher[i] = right[i];
    for (int i = 32; i < 64; i++)
        cipher[i] = left[i - 32];
    // Обратная инициализация замены
    currentBits = cipher;
    for (int i = 0; i < 64; i++)
        cipher[i] = currentBits[IP_1[i] - 1];

    return cipher;
}

bitset<64> DES::DES_decrypt(bitset<64>& cipher) {
    bitset<64> plain;
    bitset<64> currentBits;
    bitset<32> left;
    bitset<32> right;
    bitset<32> newLeft;
    // Заменить IP
    for (int i = 0; i < 64; i++)
        currentBits[i] = cipher[IP[i] - 1];

    for (int i = 0; i < 32; i++)
        left[i] = currentBits[i];
    for (int i = 32; i < 64; i++)
        right[i - 32] = currentBits[i];
    // Вводим 16 итераций (подключи применяются в обратном порядке)
    for (int round = 0; round < 16; round++) {
        newLeft = right;
        right = left ^ F(right, subkey[15 - round]);
        left = newLeft;
    }
    // Слияние
    for (int i = 0; i < 32; i++)
        plain[i] = right[i];
    for (int i = 32; i < 64; i++)
        plain[i] = left[i - 32];
    // Обратная инициализация замены
    currentBits = plain;
    for (int i = 0; i < 64; i++)
        plain[i] = currentBits[IP_1[i] - 1];

    return plain;
}

bitset<64> DES::charToBit(const char s[8]) {
    return bitset<64>(*reinterpret_cast<const unsigned long long*>(s));
}

string DES::encryptAnyString(string plaintext) {
    key = charToBit(k.c_str());
    stringstream result;
    plaintext += string(8 - (plaintext.size() % 8), '\0');
    for (size_t current = 0; current < plaintext.size() / 8; current++) {
        string tmp = plaintext.substr(8 * current, 8);
        bitset<64> plain = charToBit(tmp.c_str());
        generateKeys();
        bitset<64> cipher = DES_encrypt(plain);
        result << string(reinterpret_cast<char *>(std::addressof(cipher)), 8);
    }

    return result.str();
}

string DES::decryptAnyString(string cipher) {
    key = charToBit(k.c_str());
    stringstream result;

    for (size_t current = 0; current < cipher.size() / 8; current++) {
        bitset<64> cipher_set = charToBit(cipher.substr(8 * current, 8).c_str());
        generateKeys();
        bitset<64> plain = DES_decrypt(cipher_set);
        plain = change(plain);
        string res = plain.to_string();
        for (int i = 0; i < 8; i++) {
            string tmp = plain.to_string().substr(0 + 8 * i, 8);
            reverse(tmp.begin(), tmp.end());
            res.replace(res.begin() + i * 8, res.begin() + 8 * (i + 1), tmp);
        }
        plain = bitset<64>(res);
        result << string(reinterpret_cast<char *>(&plain), 8);
    }

    return result.str();
}
