
#include <string.h>
#include <unistd.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

int main(int argc, char *argv[]) {

  FILE *inputFile = stdin;
  FILE *outputFile = stdout;
  char *pubKeyFile = NULL; // NULL = rsa.pub
  char verbose = 0;

  // 1. getopt() 接command line看要做什麼
  /*
      -i (default: stdin): specifies the input file to encrypt.
      -o (default: stdout): specifies the output file to encrypt.
      -n (default: rsa.pub): specifies the file containing the public key.
      -v : enables verbose output.
      -h : displays program synopsis and usage.
  */
  int cmdOpt; // output of getopt
  while ((cmdOpt = getopt(argc, argv, "i:o:n:vh")) != -1) {
    switch (cmdOpt) {
    case 'i':
      inputFile = fopen(optarg, "r");
      break;
    case 'o':
      outputFile = fopen(optarg, "w");
      break;
    case 'n':
      pubKeyFile = (char *)malloc((strlen(optarg) + 1) * sizeof(char));
      strcpy(pubKeyFile, optarg);
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      fprintf(stderr, "[SYNOPSIS]\n");
      fprintf(stderr, "The program encrypts the data by the public key.\n\n");
      fprintf(stderr, "[Usage]\n");
      fprintf(stderr,
              "./encrypt [-i inputFile] [-o outputFile] [-n pubfile] [-vh]\n");
      fprintf(stderr,
              "-i (default: stdin): specifies the input file to encrypt.\n");
      fprintf(stderr,
              "-o (default: stdout): specifies the output file to encrypt.\n");
      fprintf(stderr, "-n (default: rsa.pub): specifies the file containing "
                      "the public key.\n");
      fprintf(stderr, "-v : enables verbose output.\n");
      fprintf(stderr, "-h : displays program synopsis and usage.\n");
      return 0;
      break;
    case '?':
      printf("Unknown option: %c\n", (char)optopt);
      break;
    }
  }

  // 2. fopen() public key 記得例外處理
  FILE *pubKey;
  if (pubKeyFile == NULL)
    pubKey = fopen("rsa.pub", "r");
  else
    pubKey = fopen(pubKeyFile, "r");

  // 3. read the key
  mpz_t n, e, s;
  mpz_inits(n, e, s, NULL);
  char userName[65536];
  rsa_read_pub(n, e, s, userName, pubKey);

  // 4. if -v print the following to stderr
  /*
      (a) username \n
      (b) the signature s \n
      (c) the public modulus n \n
      (d) the public exponent e \n
  */
  size_t numbits;
  if (verbose == true) {
    gmp_printf("user = %s\n", userName);
    numbits = mpz_sizeinbase(s, 2);
    gmp_printf("s (%d bits) = %Zd\n", numbits, s);
    numbits = mpz_sizeinbase(n, 2);
    gmp_printf("n (%d bits) = %Zd\n", numbits, n);
    numbits = mpz_sizeinbase(e, 2);
    gmp_printf("e (%d bits) = %Zd\n", numbits, e);
  }

  // 5. convert the username that was read in to an mpz_t. This will be the
  // expected value of the verified
  //    signature. Verify the signature using rsa_verify(), reporting an error
  //    and exiting the program if the signature couldn’t be verified
  mpz_t m;
  mpz_inits(m, NULL);
  mpz_set_str(m, userName, 62);
  if (rsa_verify(m, s, e, n) == false) {
    gmp_printf("invalid singature\n");
    fclose(inputFile);
    fclose(outputFile);
    fclose(pubKey);
    mpz_clears(n, e, s, NULL);
    return 0;
  }

  // 6. encrypt the file using rsa_encrypt_file()
  rsa_encrypt_file(inputFile, outputFile, n, e);
  fclose(inputFile);
  fclose(outputFile);
  fclose(pubKey);

  // 7. close the public key file and clear any mpz_t variables
  mpz_clears(n, e, s, m, NULL);

  return 0;
}
