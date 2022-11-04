#include <string.h>
#include <unistd.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

int main(int argc, char *argv[]) {

  FILE *inputFile = stdin;
  FILE *outputFile = stdout;
  char privKeyFile[128] = "rsa.priv";
  char verbose = 0;

  // 1. getopt() 接command line看要做什麼
  /*
      -i (default: stdin): specifies the input file to decrypt.
      -o (default: stdout): specifies the output file to decrypt.
      -n (default: rsa.priv): specifies the file containing the private key.
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
      memset(privKeyFile, '\0', 128);
      strcpy(privKeyFile, optarg);
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      fprintf(stderr, "[SYNOPSIS]\n");
      fprintf(stderr, "The program decrypts the data by the private key.\n\n");
      fprintf(stderr, "[Usage]\n");
      fprintf(stderr,
              "./decrypt [-i inputFile] [-o outputFile] [-n privfile] [-vh]\n");
      fprintf(stderr,
              "-i (default: stdin): specifies the input file to decrypt.\n");
      fprintf(stderr,
              "-o (default: stdout): specifies the output file to decrypt.\n");
      fprintf(stderr, "-n (default: rsa.priv): specifies the file containing "
                      "the private key.\n");
      fprintf(stderr, "-v : enables verbose output.\n");
      fprintf(stderr, "-h : displays program synopsis and usage.\n");
      return 0;
      break;
    case '?':
      printf("Unknown option: %c\n", (char)optopt);
      break;
    }
  }

  // 2. fopen() private key 記得例外處理
  FILE *privKey;
  privKey = fopen(privKeyFile, "r");

  // 3. read the key
  mpz_t n, d;
  mpz_inits(n, d, NULL);
  rsa_read_priv(n, d, privKey);

  // 4. if -v print the following to stderr
  /*
      (a) the public modulus n \n
      (b) the private key d \n
  */
  size_t numbits;
  if (verbose == true) {
    numbits = mpz_sizeinbase(n, 2);
    gmp_printf("n (%d bits) = %Zd\n", numbits, n);
    numbits = mpz_sizeinbase(d, 2);
    gmp_printf("d (%d bits) = %Zd\n", numbits, d);
  }

  // 5. decrypt the file using rsa_decrypt_file()
  rsa_decrypt_file(inputFile, outputFile, n, d);

  // 6. close the private key file and clear any mpz_t variables you have used
  mpz_clears(n, d, NULL);
  fclose(inputFile);
  fclose(outputFile);
  fclose(privKey);

  return 0;
}
