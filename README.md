# libmontmul
C library for performing fast modular multiplication. This library provides an implementation of [Montgomery modular multiplication](https://en.wikipedia.org/wiki/Montgomery_modular_multiplication), which eliminates the need for integer division. It works by way of an auxiliary modulus, _R_=2&#8310;&#8308;.

This was initially implemented for my fourth-year university project, and is the first step in a reimplementation thereof.
This project is intended to be used when modular arithmetic is required modulo every modulus within a certain range.
This iteration is intended to be performed by iterating over prime moduli, and constructing composite moduli therefrom.

## ```struct libmontmul_t```
```libmontmul``` accomplishes the above by using ```struct libmontmul_t```. Its contents are as follows:
* ```uint64_t modulus```: this stores the modulus under consideration, say, _N_.
* ```uint64_t neginv```: this stores a residue (say, _N'_) modulo _R_ such that *NN'*&#8801;&minus;1 modulo _R_.
* ```uint64_t auxmodsq```: this stores the residue *R*&sup2; modulo _N_.
* ```uint64_t totient```: this stores the result of Euler's [totient function](https://en.wikipedia.org/wiki/Euler%27s_totient_function) on the modulus. Useful for speeding up calculations, including exponentiation and modular inverses.

## Creating new ```struct libmontmul_t``` variables
* ```struct libmontmul_t makeLibmontmul_t(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq)```
  * Takes as input the values required for a ModEntry and combines them into a ModEntry. Performs no checks on them whatsoever.
* ```struct libmontmul_t primeLibmontmul_t(uint64_t prime);```
  * Takes as input a prime number and returns the correct corresponding ModEntry. Performs no primality check on the input.
* ```struct libmontmul_t powerLibmontmul_t(struct libmontmul_t power, struct libmontmul_t prime)```
  * Takes as input a prime power and a prime`struct libmontmul_t`, and increments the exponent of the prime power, calculating the correct values.
* ```struct libmontmul_t compositeLibmontmul_t(struct libmontmul_t multiplier, struct libmontmul_t multiplicand)```
  * Takes as input two ```struct libmontmul_t``` arguments and 'multiplies' them, returning the result. (This gives the valid ```struct libmontmul_t``` associated with the product of the moduli of the two input ```struct libmontmul_t``` arguments.) Only works as intended if the moduli of the inputs are coprime.

## Arithmetic
* ```uint64_t addmod(uint64_t augend, uint64_t addend, uint64_t modulus)```
  * Performs modular addition modulo ```modulus```.
* ```uint64_t submod(uint64_t minuend, uint64_t subtrahend, uint64_t modulus)```
  * Performs modular subtraction modulo ```modulus```.
* ```uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t)```
  * Performs a montgomery multiplication on two integers modulo ```montmul_t.modulus```. This need not be given integers already in Montgomery form.
* ```uint64_t intermul(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t)```
  * Performs a montgomery multiplication on two integers already in Montgomery form. It does not convert them back from Montgomery form.
* ```uint64_t montexp(uint64_t base, uint64_t exponent, struct libmontmul_t montmul_t)```
  * Performs modular exponentiation using an iterative binary exponentiation implementation, modulo ```montmul_t.modulus```. This need not be given a base already in Montgomery form.
* ```uint64_t sqrtmod(uint64_t residue, struct libmontmul_t montmul_t)```
  * Finds a modular square root of the input residue modulo ```montmul_t.modulus```. This does not perform a quadratic residue test and is only inteded to be run on quadratic residues with prime ModEntry.

## Number-theoretic
* ```uint64_t invmod(uint64_t residue, struct libmontmul_t montmul_t)```
  * Given a residue coprime with ```montmul_t.modulus```, returns the modular inverse of the residue.
* ```int8_t legendre(uint64_t residue, struct libmontmul_t montmul_t)```
  * Calculates the [Legendre symbol](https://en.wikipedia.org/wiki/Legendre_symbol) of the residue with regard to the ```montmul_t.modulus```.
* ```uint64_t crtCalc(uint64_t residue1, struct libmontmul_t multiplier, uint64_t residue2, struct libmontmul_t multiplicand, struct libmontmul_t product)```
  * Uses the Chinese remainder theorem to calculate the unique residue modulo ```prouct.modulus``` congruent to ```residue1``` modulo ```multiplier.modulus``` and to ```residue2``` modulo ```multiplicand.modulus```. Only works if the moduli are coprime.

## Comparison
* ```bool equalsLibmontmul_t(struct libmontmul_t montmul_t1, struct libmontmul_t montmul_t2)```
  * Returns true if the operands are equal (i.e., their values are identitcal), and false otherwise.

## Requirements
CMake with GCC is required to compile the tests.
For compiling the tests on Windows, see [WINDOWS.md](WINDOWS.md).
