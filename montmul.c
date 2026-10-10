#include "montmul.h"

uint64_t addmod(uint64_t augend, uint64_t addend, uint64_t modulus) {
	const uint64_t SUM = augend + addend;
	return (SUM >= modulus) ? (SUM - modulus) : SUM;
}

uint64_t submod(uint64_t minuend, uint64_t subtrahend, uint64_t modulus) {
	return (subtrahend > minuend) ? minuend + modulus - subtrahend : minuend - subtrahend;
}

uint64_t REDC(__uint128_t operand, struct libmontmul_t montmul_t) {
	const uint64_t modulus = montmul_t.modulus;
	const uint64_t m = ((__uint128_t)(operand & UINT64_MAX) * montmul_t.neginv) & UINT64_MAX;
	const uint64_t t = (operand + (__uint128_t)m * modulus) >> 64;
	return (t >= modulus) ? (uint64_t)(t - modulus) : t;
}

uint64_t toMontgomeryForm(uint64_t operand, struct libmontmul_t montmul_t) {
	return REDC((__uint128_t)operand * montmul_t.auxmodsq, montmul_t);
}

uint64_t fromMontgomeryForm(uint64_t operand, struct libmontmul_t montmul_t) {
	return REDC(operand, montmul_t);
}

uint64_t montmulpwr2(uint64_t multiplier, uint64_t multiplicand, uint64_t modulus) {
	return ((__uint128_t)multiplier * multiplicand) & (modulus - 1);
}

uint64_t montmulodd(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t) {
	multiplier = toMontgomeryForm(multiplier, montmul_t);
	multiplicand = toMontgomeryForm(multiplicand, montmul_t);
	const uint64_t result = REDC((__uint128_t)multiplier * multiplicand, montmul_t);
	return fromMontgomeryForm(result, montmul_t);
}

uint64_t semiCRT(uint64_t res1, struct libmontmul_t montmul_t1, uint64_t res2, struct libmontmul_t montmul_t2, uint64_t modr);

uint64_t montmuleven(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t) {
	const uint64_t modulus = montmul_t.modulus;
	const uint64_t pwr2 = __builtin_ctzll(modulus);
	const uint64_t modpwr2 = (1ULL << pwr2);
	const uint64_t odd = modulus >> pwr2;
	const struct libmontmul_t oddEntry = makeLibmontmul_t(odd, montmul_t.neginv, montmul_t.auxmodsq, montmul_t.totient);
	const uint64_t result1 = montmulpwr2(multiplier, multiplicand, modpwr2);
	const uint64_t result2 = montmulodd(multiplier, multiplicand, oddEntry);
	return semiCRT(result1, makeLibmontmul_t(modpwr2, 0, 0, 0), result2, oddEntry, modulus);
}

uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t) {
	const uint64_t modulus = montmul_t.modulus;
	if (__builtin_popcountll(modulus) == 1) {
		// modulus is a power of 2.
		return montmulpwr2(multiplier, multiplicand, modulus);
	}
	if ((modulus & 0b1) == 0) {
		// modulus is an even number but not a power of 2.
		return montmuleven(multiplier, multiplicand, montmul_t);
	}
	// modulus is odd.
	return montmulodd(multiplier, multiplicand, montmul_t);
}

uint64_t montexppwr2(uint64_t base, uint64_t exponent, uint64_t modulus) {
	const uint64_t bitmask = modulus - 1;
	uint64_t power = 1;
	while (exponent > 0) {
		if ((exponent & 0b1) == 1) {
			power = ((__uint128_t)power * base) & bitmask;
		}
		base = ((__uint128_t)base * base) & bitmask;
		exponent >>= 1;
	}
	return power;
}

uint64_t intermul(uint64_t multiplier, uint64_t multiplicand, struct libmontmul_t montmul_t) {
	return REDC((__uint128_t)multiplier * multiplicand, montmul_t);
}

uint64_t montexpodd(uint64_t base, uint64_t exponent, struct libmontmul_t montmul_t) {
	const uint64_t auxmodsq = montmul_t.auxmodsq;
	uint64_t power = REDC(auxmodsq, montmul_t);
	base = toMontgomeryForm(base, montmul_t);
	while (exponent > 0) {
		if ((exponent & 0b1) == 1) {
			power = intermul(power, base, montmul_t);
		}
		base = intermul(base, base, montmul_t);
		exponent >>= 1;
	}
	return fromMontgomeryForm(power, montmul_t);
}

uint64_t montexpeven(uint64_t base, uint64_t exponent, struct libmontmul_t montmul_t) {
	const uint64_t modulus = montmul_t.modulus;
	const uint64_t pwr2 = __builtin_ctzll(modulus);
	const uint64_t modpwr2 = (1ULL << pwr2);
	const uint64_t odd = modulus >> pwr2;
	const struct libmontmul_t oddEntry = makeLibmontmul_t(odd, montmul_t.neginv, montmul_t.auxmodsq, montmul_t.totient);
	const uint64_t result1 = montexppwr2(base, exponent, modpwr2);
	const uint64_t result2 = montexpodd(base, exponent, oddEntry);
	return semiCRT(result1, makeLibmontmul_t(modpwr2, 0, 0, 0), result2, oddEntry, modulus);
}

uint64_t montexp(uint64_t base, uint64_t exponent, struct libmontmul_t montmul_t) {
	const uint64_t modulus = montmul_t.modulus;
	if (__builtin_popcountll(modulus) == 1) {
		// modulus is a power of 2.
		return montexppwr2(base, exponent, modulus);
	}
	if ((modulus & 0b1) == 0) {
		// modulus is an even number but not a power of 2.
		return montexpeven(base, exponent, montmul_t);
	}
	// modulus is odd.
	return montexpodd(base, exponent, montmul_t);
}

uint64_t naiveexp(uint64_t base, uint64_t exponent, uint64_t modulus) {
	uint64_t power = 1;
	while (exponent > 0) {
		if ((exponent & 0b1) == 1) {
			power = power * base;
		}
		base = base * base;
		exponent >>= 1;
	}
	return power % modulus;
}

uint64_t invmod(uint64_t residue, struct libmontmul_t montmul_t) {
	const uint64_t totient = montmul_t.totient;
	return montexp(residue, totient - 1, montmul_t);
}

int8_t legendre(uint64_t residue, struct libmontmul_t montmul_t) {
	const uint64_t prime = montmul_t.modulus;
	const uint64_t power = montexp(residue, (prime-1)/2, montmul_t);
	return (power > 1) ? -1 : (int8_t)power;
}

uint64_t semiCRT(uint64_t res1, struct libmontmul_t montmul_t1, uint64_t res2, struct libmontmul_t montmul_t2, uint64_t modr) {
	uint64_t mod1 = montmul_t1.modulus;
	uint64_t mod2 = montmul_t2.modulus;
	if (mod1 < mod2) {
		{
			uint64_t t = mod2;
			mod2 = mod1;
			mod1 = t;
		}
		{
			uint64_t t = res2;
			res2 = res1;
			res1 = t;
		}
		const struct libmontmul_t TEMP = montmul_t2;
		montmul_t2 = montmul_t1;
		montmul_t1 = TEMP;
	}

	for (uint64_t i = 0; res1 + i * mod1 < modr; i++) {
		if (montmul(res1 + i * mod1, 1, montmul_t2) != res2) {continue;}
		return res1 + i * mod1;
	}
	return 0;
}

struct libmontmul_t makeLibmontmul_t(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq, uint64_t totient) {
	return (struct libmontmul_t){modulus, neginv, auxmodsq, totient};
}

uint64_t calcAuxmodsq(uint64_t modulus) {
	const uint64_t auxmodsq = ~((__uint128_t)0) % modulus + 1;
	return (auxmodsq == modulus) ? 0 : auxmodsq;
}

uint64_t calcNeginv(uint64_t modulus) {
	uint64_t result = 1;
	uint64_t bitmask = 1;
	for (uint8_t i = 1; i <= 64; i++) {
		bitmask = (bitmask << 1) + 1;
		result = - (((__uint128_t)(modulus - 1) * result + 1) & bitmask);
	}
	return result;
}

struct libmontmul_t primeLibmontmul_t(uint64_t prime) {
	if (prime != 2) {
		return makeLibmontmul_t(prime,
			calcNeginv(prime),
			calcAuxmodsq(prime),
			prime - 1);
	}
	return makeLibmontmul_t(2, 0, 0, 1);
}

// We assume the prime of the prime power to be the same as that given as `prime.'
struct libmontmul_t powerLibmontmul_t(struct libmontmul_t power, struct libmontmul_t prime) {
	const uint64_t oldModulus = power.modulus;
	const uint64_t newModulus = oldModulus * prime.modulus;
	const uint64_t newNeginv = -power.neginv * prime.neginv;
	const uint64_t newAuxmodsq = calcAuxmodsq(newModulus);
	const uint64_t newTotient = newModulus - oldModulus;
	return makeLibmontmul_t(newModulus, newNeginv, newAuxmodsq, newTotient);
}

// We assume at most one of the operands to bear an even modulus.
struct libmontmul_t compositeLibmontmul_t(struct libmontmul_t multiplier, struct libmontmul_t multiplicand) {
	const uint64_t modulus1 = multiplier.modulus;
	const uint64_t modulus2 = multiplicand.modulus;
	const uint64_t modulusr = modulus1 * modulus2;

	const uint64_t neginv1 = multiplier.neginv;
	const uint64_t neginv2 = multiplicand.neginv;

	const uint64_t auxmodsq1 = multiplier.auxmodsq;
	const uint64_t auxmodsq2 = multiplicand.auxmodsq;

	const uint64_t totient1 = multiplier.totient;
	const uint64_t totient2 = multiplicand.totient;

	if ((modulus1 & 0b1) == 0) {
		return makeLibmontmul_t(modulusr, neginv2, auxmodsq2, totient2);
	}
	if ((modulus2 & 0b1) == 0) {
		return makeLibmontmul_t(modulusr, neginv1, auxmodsq1, totient1);
	}

	const uint64_t neginvr = -neginv1 * neginv2;
	const uint64_t auxmodsqr = semiCRT(auxmodsq1, multiplier, auxmodsq2, multiplicand, modulusr);
	const uint64_t totientr = totient1 * totient2;
	return makeLibmontmul_t(modulusr, neginvr, auxmodsqr, totientr);
}

bool equalsLibmontmul_t(struct libmontmul_t montmul_t1, struct libmontmul_t montmul_t2) {
	return montmul_t1.modulus == montmul_t2.modulus &&
		montmul_t1.neginv == montmul_t2.neginv &&
		montmul_t1.auxmodsq == montmul_t2.auxmodsq &&
		montmul_t1.totient == montmul_t2.totient;
}

// sqrt

uint64_t leastPwr2(uint64_t t, uint64_t M, struct libmontmul_t montmul_t) {
	for (uint64_t i = 1; i < M; i++) {
		t = montexp(t, 2, montmul_t);
		if (t != 1) {continue;}
		return i;
	}
	return M;
}

uint64_t findQuadNonres(struct libmontmul_t montmul_t) {
	const uint64_t PRIME = montmul_t.modulus;
	for (uint64_t i = 2; i < PRIME; i++) {
		if (legendre(i, montmul_t) >= 0) {continue;}
		return i;
	}
	return 0; //error case. on an odd prime input this should never happen.
}

uint64_t tonellishanks(uint64_t residue, struct libmontmul_t montmul_t) {
	const uint64_t PRIME = montmul_t.modulus;
	const uint64_t PRIME_MINUS_ONE = PRIME - 1;
	uint64_t z, c, t, R, i, b, M, Q;
	M = __builtin_ctzll(PRIME_MINUS_ONE);
	Q = (PRIME_MINUS_ONE) >> M;
	z = findQuadNonres(montmul_t);
	c = montexp(z, Q, montmul_t);
	t = montexp(residue, Q, montmul_t);
	R = montexp(residue, (Q+1)/2, montmul_t);
	while (t > 0) {
		if (t == 1) {return R;}
		i = leastPwr2(t, M, montmul_t);
		b = montexp(c, naiveexp(2, M-i-1, PRIME_MINUS_ONE), montmul_t);
		M = i;
		c = montexp(b, 2, montmul_t);
		t = montmul(t, c, montmul_t);
		R = montmul(R, b, montmul_t);
	}
	return 0;
}

uint64_t sqrtmod(uint64_t residue, struct libmontmul_t montmul_t) {
	if (residue <= 1) {return residue;}
	uint64_t PRIME = montmul_t.modulus;
	if ((PRIME & 0b11) == 3) {
		return montexp(residue, (PRIME+1)/4, montmul_t);
	}
	return tonellishanks(residue, montmul_t);
}

uint64_t crtCalc(uint64_t residue1, struct libmontmul_t multiplier, uint64_t residue2, struct libmontmul_t multiplicand, struct libmontmul_t product) {
	const uint64_t MOD1 = multiplier.modulus;
	const uint64_t MOD2 = multiplicand.modulus;
    const uint64_t INV1 = invmod(MOD1, multiplicand);
    const uint64_t INV2 = invmod(MOD2, multiplier);
    return addmod(
        montmul(residue1, montmul(MOD2, INV2, product), product),
        montmul(residue2, montmul(MOD1, INV1, product), product), product.modulus);
}