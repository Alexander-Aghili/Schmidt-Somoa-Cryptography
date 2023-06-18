# Schmidt-Samoa Cryptography
## Short Description
This is a program that can fully encrypt and decrypt messages according to a public/private key pair. 
There are three executable components:
1. A key pair can be generated using the keygen program
2. A message can be encrypted using the encrypt program and the generated public key file
3. An encrypted message can be decrypted to the original message using the decrypt program and the previously generated private key file

## GNU Multi-Precision Library
To safely encrypt messages, the sizes of the integers used exceeds 64 bits of precision, the maximum for default C types. 
As a result, the GNU Multi-Precision Library (GMP for short) is utilized. Due to added complexity, comments describing operations in plain C are often incorporated next to the used GMP functions. The library for GMP can be found here:
[GMP Library](https://gmplib.org/manual/)

## How to Build
To build, you must have the Makefile. This operates by collecting the C files, generating the object files, and linking them into the binary executable. You must have all of the .c and .h files from this repository to build. Once you have all the appropriate files, you can build each executable independantly or all together at once. To build all the files:
```
make
```
or 
```
make all
```
To make each executable independantly:
```
make keygen
make encrypt
make decrypt
```
To see the command line arguments for each executable, run the following commands or see below.
```
./keygen -h
./encrypt -h
./decrypt -h 
```

## Keygen Command Line Arguments
- -b *bits*: Makes public key greater than or equal to *bits* number of bits (Default: 256 bits)
- -i *iters*: Specifies *iters* number of iterations for Miller-Rabin primality test (Default: 50 iterations)
- -n *pbfile*: Specifies *pbfile* to store public key (Default: ss.pub)
- -d *pvfile*: Specifies *pvfile* to store private keys (Default: ss.priv)
- -s *seed*: Specifies seed for random state initializations, used for testing purposes only (Default: current UNIX epoch time)
- -v: Enables verbose program output
- -h: Prints help usage

## Decrypt/Encrypt Command Line Arguments
Encrypt and decrypt share the same command line arguments detailed below:
- -i *infile*: Specifies input file as *infile*. (Default: stdin)
- -o *outfile*: Specifies outputfile as *outfile*. (Default: stdout)
- -n *keyfile*: Specifies public key file in case of encrypt and private key file in case of decrypt. (Default: ss.pub (encrypt) or ss.priv (decrypt))
- -v: Enables verbose program output
- -h: Prints help usage

## To Run
The following is an example of how to encrypt a message in *input.txt* and output that encrypted message to *encrypted_message.txt*. It will then decrypt that encrypted message into *output.txt*. Other inputs will be default.

First, generate the keys:
```
./keygen
```
A 256 bit public/private key pair was created and put into ss.pub and ss.priv.

Then, encrypt the message in *input.txt*:
```
./encrypt -i input.txt -o encrypted_message.txt
```
Now an encrypted message is located in *encrypted_message.txt*.

Finally, decrypt that encrypted message and place the output into *output.txt*:
```
./decrypt -i encrypted_message.txt -o output.txt
```
Now the message in *input.txt* and *output.txt* are the same. 
