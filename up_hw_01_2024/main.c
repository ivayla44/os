#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>


int get_last_digit(int number) {
    return number % 10;
}

uint16_t mark_digit(int digit) {
    uint16_t marker = 1;

    for (int i = 0; i < digit; i++) {
        marker *= 2;
    }

    return marker;
}

uint16_t extract_last_digit_information(int digit) {
    return mark_digit(digit);
}

int is_digit(uint8_t digit){
    return digit < 10;
}

int is_digit_marker_pressent(uint16_t markers, uint16_t marker) {
    return markers & marker;
}

int is_digit_marked(uint16_t markers, int digit) {
   return is_digit_marker_pressent(markers, mark_digit(digit));
}

void print_digit(int digit){
    printf("Next common digit: %d\n", digit);
}

uint16_t extract_digits_information(int input) {
    /* Use 10 of the following 16 bytes to hold information about the digits making up the first number. */
    uint16_t digits_information = 0;
    /* The 10 least significant bits will indicate the pressence of a digit, 0 to 9. */
    
    // Edge case, input is 0.
    
    // iva: could just do-while instead?
    
    if (input == 0) {
        digits_information = extract_last_digit_information(get_last_digit(0));
    }
    
    while ( input > 0 ) {
        digits_information |= extract_last_digit_information(get_last_digit(input));
        input /= 10;
    }

    return digits_information;
}

int main(int argc, char * argv[]) {
    // Handle input:
    int input_first;
    int intput_second;

    scanf("%d", &input_first);
    scanf("%d", &intput_second);
    // Input handled.

    // iva: probs just do absolute values; "-1234" and "123" wouldn't return any common digits...

    uint16_t digits_first = extract_digits_information(input_first);
    uint16_t digits_second = extract_digits_information(intput_second);

    // 2 bytes beauty tax to avoid:
    // digits_first &= digits_second

    // iva: cute :3
    
    uint16_t common_digits = digits_first & digits_second;

    for (int digit = 0;  is_digit(digit); digit++) {
        if ( is_digit_marked(common_digits, digit) ) {
            print_digit(digit);
        }
    }

    return 0;
}
