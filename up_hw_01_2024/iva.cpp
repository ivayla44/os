#include <iostream>

uint32_t bitmap(int a) {
    uint32_t bitmap = 0;
    a = std::abs(a);

    uint32_t digit;
    do {
        digit = a % 10;
        bitmap = bitmap | (1 << digit);
        a /= 10;
    } while(a);

    return bitmap;
}

void commonDigits(int a, int b) {
    uint32_t bitmap_a = bitmap(a);
    uint32_t bitmap_b = bitmap(b);

    uint32_t common_digits = bitmap_a & bitmap_b;
    if(common_digits == 0) {
        std::cout << "none";
    }
    else {
        for(int i = 0; i < 10; i++) {
            if(common_digits & (1 << i)) {
                std::cout << i;
            }
        }
    }
}

// had a main w tests for all the tasks of the hw, too lazy to write a pretty one for this
