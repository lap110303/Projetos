#include <iostream>


void sum(int a, int b) {
    std::cout << "Sum of int: " << (a + b) << std::endl;
}
void sum(double a, double b) {
    std::cout << "Sum of double: " << (a + b) << std::endl;
}

int main() {
    sum(1, 2);
    sum(1.1, 2.2);
    sum(1, 2);
    sum((int) 1.1, (int) 2.2);
    return 0;
} 
