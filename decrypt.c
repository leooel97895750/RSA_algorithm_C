#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

int main() {

    // 1. getopt() 接command line看要做什麼
    /*
        -i (default: stdin): specifies the input file to decrypt
        -o (default: stdout): specifies the output file to decrypt
        -n (default: rsa.priv): specifies the file containing the private key
        -v : enables verbose output
        -h : displays program synopsis and usage
    */

    // 2. fopen() private key 記得例外處理

    // 3. read the key

    // 4. if -v print the following to stderr
    /*
        (a) the public modulus n \n
        (b) the private key d \n
    */

    // 5. decrypt the file using rsa_decrypt_file()

    // 6. close the private key file and clear any mpz_t variables you have used

    return 0;
}