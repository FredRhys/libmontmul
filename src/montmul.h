#ifndef MONTMUL_H

#define MONTMUL_H
#include <inttypes.h>
#include <stdbool.h>

struct libmontmul_t{
	uint64_t modulus;
	uint64_t neginv;
	uint64_t auxmodsq;
	uint64_t totient;
};

struct libmontmul_t makeLibmontmul_t(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq, uint64_t totient);
struct libmontmul_t primeLibmontmul_t(uint64_t prime);
struct libmontmul_t powerLibmontmul_t(struct libmontmul_t power, struct libmontmul_t prime);
struct libmontmul_t compositeLibmontmul_t(struct libmontmul_t multiplier, struct libmontmul_t multiplicand);
uint64_t addmod(uint64_t augend, uint64_t addend, uint64_t modulus);
uint64_t submod(uint64_t minuend, uint64_t subtrahend, uint64_t modulus);
uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t);
uint64_t intermul(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t);
uint64_t montexp(uint64_t base, uint64_t exponent, struct libmontmul_t montmul_t);
uint64_t invmod(uint64_t residue, struct libmontmul_t montmul_t);
uint64_t sqrtmod(uint64_t residue, struct libmontmul_t montmul_t);
int8_t legendre(uint64_t residue, struct libmontmul_t montmul_t);
bool equalsLibmontmul_t(struct libmontmul_t montmul_t1, struct libmontmul_t montmul_t2);
uint64_t crtCalc(uint64_t residue1, struct libmontmul_t multiplier, uint64_t residue2, struct libmontmul_t multiplicand, struct libmontmul_t newEntry);

#endif