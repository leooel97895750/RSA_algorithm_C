#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <gmp.h>

#include "randstate.h"
#include "rsa.h"

int main(int argc, char *argv[]) {

    uint64_t nbits = 1024;
    uint64_t iters = 50;
    char *pubKeyFile = NULL; // NULL = rsa.pub
    char *privKeyFile = NULL; // NULL = rsa.priv
    uint64_t timeSeed = time(NULL);
    char verbose = 0;

    // 1. getopt() 接command line看要做什麼
    /*
        -b (default 1024): specifies the minimum bits needed for the public modulus n
        -i (default: 50): specifies the number of Miller-Rabin iterations for testing primes
        -n (default: rsa.pub): specifies the public key file
        -d (default: rsa.priv): specifies the private key file
        -s (default: time seed): specifies the random seed for the random state initialization
        -v : enables verbose output
        -h : displays program synopsis and usage.
    */
    int cmdOpt; // output of getopt
    char *ptr; // for strtoull
    while ((cmdOpt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1){
        switch (cmdOpt) {
            case 'b':
                nbits = atoi(optarg);
                break;
            case 'i':
                iters = atoi(optarg);
                break;
            case 'n':
                pubKeyFile = (char*) malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(pubKeyFile, optarg);
                break;
            case 'd':
                privKeyFile = (char*) malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(privKeyFile, optarg);
                break;
            case 's':
                timeSeed = strtoul(optarg, &ptr, 10);
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
    printf("nbits: %lu \n", nbits);
    printf("iters: %lu \n", iters);
    printf("timeSeed: %lu \n", timeSeed);

    // 2. fopen() public or private key 記得例外處理
    FILE *pubKey, *privKey;
    if (pubKeyFile == NULL) pubKey = fopen("rsa.pub", "w");
    else pubKey = fopen(pubKeyFile, "w");
    if (privKeyFile == NULL) privKey = fopen("rsa.priv", "w");
    else privKey = fopen(privKeyFile, "w");

    // 3. fchmod() fileno() set to 0600 (read and write permissions for the user)
    fchmod(fileno(pubKey), 0600);
    fchmod(fileno(privKey), 0600);

    // 4. randstate_init()
    randstate_init(timeSeed);

    // 5. rsa_make_pub() rsa_make_priv()
    mpz_t p, q, n, e; // prime num: p, q; product of pq: n; public exponent: e
    mpz_inits(p, q, n, e, NULL);
    rsa_make_pub(p, q, n, e, nbits, iters);
    //rsa_make_priv(d, e, p, q);

    // 6. getenv() get the current user’s name as a string

    // 7. convert the username into an mpz_t with mpz_set_str(), specifying the base as 62. Then, use rsa_sign() to compute the signature of the username

    // 8. write the computed public and private key to their respective files

    // 9. if -v print the following to stderr
    /*
        (a) username \n
        (b) the signature s \n
        (c) the first large prime p \n
        (d) the second large prime q \n
        (e) the public modulus n \n
        (f) the public exponent e \n
        (g) the private key d \n
    */

    // 10. Close the public and private key files, randstate_clear(), and clear any mpz_t variables you may have used.
    fclose(pubKey);
    fclose(privKey);
    free(pubKeyFile);
    free(privKeyFile);

    return 0;
}
