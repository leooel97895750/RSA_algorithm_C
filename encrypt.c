#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

int main() {

    FILE *inputFile = stdin;
    FILE *outputFile = stdout;
    char *pubKeyFile = NULL; // NULL = rsa.pub
    char verbose = 0;

    // 1. getopt() 接command line看要做什麼
    /*
        -i (default: stdin): specifies the input file to encrypt
        -o (default: stdout): specifies the output file to encrypt 
        -n (default: rsa.pub): specifies the file containing the public key
        -v : enables verbose output
        -h : displays program synopsis and usage
    */
    int cmdOpt; // output of getopt
    while ((cmdOpt = getopt(argc, argv, "i:o:n:vh")) != -1){
        switch (cmdOpt) {
            case 'i':
                inputFile = fopen(optarg, "r");
                break;
            case 'o':
                outputFile = fopen(optarg, "w");
                break;
            case 'n':
                pubKeyFile = (char*) malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(pubKeyFile, optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                // print help
                return 0;
                break;
            case '?':
                printf("Unknown option: %c\n", (char)optopt);
                break;
        }
    }

    // 2. fopen() public key 記得例外處理

    // 3. read the key

    // 4. if -v print the following to stderr
    /*
        (a) username \n
        (b) the signature s \n
        (c) the public modulus n \n
        (d) the public exponent e \n
    */

    // 5. convert the username that was read in to an mpz_t. This will be the expected value of the verified \
          signature. Verify the signature using rsa_verify(), reporting an error and exiting the program if \
          the signature couldn’t be verified

    // 6. encrypt the file using rsa_encrypt_file()

    // 7. close the public key file and clear any mpz_t variables

    return 0;
}