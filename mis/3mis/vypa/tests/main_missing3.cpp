int factorial (int n) {
    int decremented_n, temp_result;
    if (n < 2) {
        return 1;
    }
    else {
        decremented_n = n - 1; temp_result = factorial(decremented_n);
    }
    return n * temp_result;
}