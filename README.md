# Basic SHA-256 & SHA-512 Implementations in C

Basic implementation of SHA-256 and SHA-512 in C. 

Use as a command line tool to hash strings.

---


`./sha256 abc`
Produces:
`ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad`


`./sha512 abc`
Produces:
`ddaf35a193617abacc417349ae20413112e6fa4e89a97ea2a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f`


`./sha256 -f myFile` or `./sha512 -f myFile`
Allows to specify a file containing a string. Important: a trailing \n is removed.

`./sha256 -v abc` or `./sha512 -v abc` Makes the output somewhat more verbose.
