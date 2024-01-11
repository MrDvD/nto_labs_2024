#include <iostream>
#include <cmath>
#include <bitset>

std::string int_to_binary(int num, bool sign) {
    std::string binarized;
    if (sign) {
        num = abs(num);
        binarized += num >= 0 ? '0' : '1';
    }
    // change '8' to desired bit count
    binarized += std::bitset<8>(num).to_string();
    return binarized;
}

int binary_to_int(const char *str, bool sign) {
    std::string binary = std::string(str);
    int result;
    if (sign) {
        int sign = binary[0] - '0';
        result = std::strtol(binary.substr(1).c_str(), 0, 2);
        if (sign > 0) {
            result *= -1;
        }
    } else {
        result = std::strtol(binary.c_str(), 0, 2);
    }
    return result;
}

std::string float_to_binary(float num, int precision) {
    std::string binarized;
    float integer;
    float fraction = std::round(std::modf(num, &integer) * pow(10, precision));
    integer = abs(integer); fraction = abs(fraction);
    if (fraction > 0) { // CUTS FRACTION'S TRAILING ZEROS
        while ((int) fraction % 10 == 0) {
            fraction /= 10;
        }
    }
    binarized += num >= 0 ? '0' : '1'; // SIGN
    if (fraction == 0) {
        binarized += "0"; // EXP_SIGN
        int zeros = 0;
        if (integer != 0) {
            while ((int) integer % 10 == 0) {
                integer /= 10;
                zeros++;
            }
        }
        binarized += std::bitset<6>(zeros).to_string(); // EXPONENT
        binarized += std::bitset<24>(integer).to_string(); // MANTISSA
    } else {
        binarized += "1"; // EXP_SIGN
        if (precision > ceil(log10(fraction))) { // CUTS MANTISSA'S TRAILING ZEROS
            precision = ceil(log10(fraction));
        }
        binarized += std::bitset<6>(precision).to_string(); // EXPONENT
        binarized += std::bitset<24>(integer * pow(10, precision) + fraction).to_string(); // MANTISSA
    }
    return binarized;
}

float binary_to_float(const char *str) {
    std::string binary = std::string(str);
    int sign = binary[0] - '0';
    int exp_sign = binary[1] - '0';
    int exponent = std::strtol(binary.substr(2, 6).c_str(), 0, 2);
    int mantissa = std::strtol(binary.substr(8, 24).c_str(), 0, 2);
    if (exp_sign > 0) {
        exponent *= -1;
    }
    float result = mantissa * pow(10, exponent);
    if (sign > 0) {
        result *= -1;
    }
    return result;
}

std::string input;
int main() {
    std::cin >> input;
    // SLICING
    std::string first = input.substr(0, 8);
    std::string second = input.substr(8, 8);
    // BINARY TO DECIMAL
    int num1 = std::strtol(first.c_str(), 0, 2);
    int num2 = std::strtol(second.c_str(), 0, 2);
    std::cout << first << " " << num1 << '\n';
    std::cout << second << " " << num2 << '\n';
    // CHAR-BY-CHAR ACCESS
    for (int i = 0; i < 8; i++) {
        std::cout << first[i] << ' ';
    }
    // INT BINARIZATION
    std::string int_binarized = int_to_binary(207, true);
    std::cout << '\n' << int_binarized << std::endl;
    std::cout << binary_to_int(int_binarized.c_str(), true) << std::endl;
    // FLOAT BINARIZATION
    std::string float_binarized = float_to_binary(36.6, 2);
    std::cout << float_binarized << std::endl;
    std::cout << binary_to_float(float_binarized.c_str());
    return 0;
}