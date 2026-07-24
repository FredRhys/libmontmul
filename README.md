# libmontmul
C library for performing fast modular multiplication. This library provides an implementation of [Montgomery modular multiplication](https://en.wikipedia.org/wiki/Montgomery_modular_multiplication), which eliminates the need for integer division. It works by way of an auxiliary modulus, $R$, which we set to $2^{64}$.

This was initially implemented for my fourth-year university project, and is the first step in a reimplementation thereof.

## ModEntries
```libmontmul``` accomplishes the above by using a ```struct``` called a ```ModEntry```. Its structure is as follows:
* ```uint64_t modulus```: this stores the modulus under consideration, say, $N$.
* ```uint64_t neginv```: this stores a residue (say, $N'$) modulo $R=2^{64}$, such that $NN'\equiv-1\pmod{R}$.
* ```uint64_t auxmodsq```: this stores the residue $R^{2}\pmod{N}$.
* ```uint64_t totient```: this stores the result of [Euler's totient function](https://en.wikipedia.org/wiki/Euler%27s_totient_function) on the modulus. Useful for speeding up calculations, including exponentiation and modular inverses.

## Creating new ModEntries
* ```ModEntry makeModEntry(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq)```
  * Takes as input the values required for a ModEntry and combines them into a ModEntry. Performs no checks on them whatsoever.
* ```ModEntry primeModEntry(uint64_t prime);```
  * Takes as input a prime number and returns the correct corresponding ModEntry. Performs no primality check on the input.
* ```ModEntry increasePrimeModEntryPower(ModEntry powerEntry, ModEntry primeEntry)```
  * Takes as input a ModEntry whose modulus is a prime power, and a ModEntry whose modulus is that prime, and increments the exponent of the prime power, calculating the correct values.
* ```ModEntry combineCoprimeModEntries(ModEntry operand1, ModEntry operand2)```
  * Takes as input two ModEntries and returns the ModEntry associated with the product of the inputs' moduli. Only works as intended if the moduli of the inputs are coprime.

## Arithmetic
* ```uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry)```
  * Performs a montgomery multiplication on two integers modulo the modulus containted in the ModEntry. This need not be given integers already in Montgomery form.
* ```uint64_t intermul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry)```
  * Performs a montgomery multiplication on two integers already in Montgomery form. It does not convert them back from Montgomery form.
* ```uint64_t montexp(uint64_t base, uint64_t exponent, ModEntry modEntry)```
  * Performs modular exponentiation using an iterative binary exponentiation implementation. This need not be given a base already in Montgomery form.
## Comparison
* ```bool modEntriesEqual(ModEntry operand1, ModEntry operand2)```
  * Returns true if the operands are equal (i.e., their values are identitcal), and false otherwise.

## Requirements
This project was tested only on Linux. Providing a working test file for Windows or Mac is outside the scope of this project.
