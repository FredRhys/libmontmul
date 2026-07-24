#include "montmul.h"

uint64_t REDC(__uint128_t operand, ModEntry modEntry) {
	const uint64_t modulus = modEntry.modulus;
	const uint64_t m = ((__uint128_t)(operand & UINT64_MAX) * modEntry.neginv) & UINT64_MAX;
	const uint64_t t = (operand + (__uint128_t)m * modulus) >> 64;
	return (t >= modulus) ? (uint64_t)(t - modulus) : t;
}

uint64_t toMontgomeryForm(uint64_t operand, ModEntry modEntry) {
	return REDC((__uint128_t)operand * modEntry.auxmodsq, modEntry);
}

uint64_t fromMontgomeryForm(uint64_t operand, ModEntry modEntry) {
	return REDC(operand, modEntry);
}

uint64_t montmulpwr2(uint64_t multiplier, uint64_t multiplicand, uint64_t modulus) {
	return ((__uint128_t)multiplier * multiplicand) & (modulus - 1);
}

uint64_t montmulodd(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry) {
	multiplier = toMontgomeryForm(multiplier, modEntry);
	multiplicand = toMontgomeryForm(multiplicand, modEntry);
	const uint64_t result = REDC((__uint128_t)multiplier * multiplicand, modEntry);
	return fromMontgomeryForm(result, modEntry);
}

uint64_t semiCRT(uint64_t res1, ModEntry entry1, uint64_t res2, ModEntry entry2, uint64_t modr);

uint64_t montmuleven(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry) {
	const uint64_t modulus = modEntry.modulus;
	const uint64_t pwr2 = __builtin_ctzll(modulus);
	const uint64_t modpwr2 = (1ULL << pwr2);
	const uint64_t odd = modulus >> pwr2;
	const ModEntry oddEntry = makeModEntry(odd, modEntry.neginv, modEntry.auxmodsq);
	const uint64_t result1 = montmulpwr2(multiplier, multiplicand, modpwr2);
	const uint64_t result2 = montmulodd(multiplier, multiplicand, oddEntry);
	return semiCRT(result1, (ModEntry){modpwr2, 0, 0}, result2, oddEntry, modulus);
}

uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry) {
	const uint64_t modulus = modEntry.modulus;
	if (__builtin_popcountll(modulus) == 1) {
		// modulus is a power of 2.
		return montmulpwr2(multiplier, multiplicand, modulus);
	}
	if ((modulus & 0b1) == 0) {
		// modulus is an even number but not a power of 2.
		return montmuleven(multiplier, multiplicand, modEntry);
	}
	// modulus is odd.
	return montmulodd(multiplier, multiplicand, modEntry);
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

uint64_t intermul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry) {
	return REDC((__uint128_t)multiplier * multiplicand, modEntry);
}

uint64_t montexpodd(uint64_t base, uint64_t exponent, ModEntry modEntry) {
	const uint64_t auxmodsq = modEntry.auxmodsq;
	uint64_t power = REDC(auxmodsq, modEntry);
	base = toMontgomeryForm(base, modEntry);
	while (exponent > 0) {
		if ((exponent & 0b1) == 1) {
			power = intermul(power, base, modEntry);
		}
		base = intermul(base, base, modEntry);
		exponent >>= 1;
	}
	return fromMontgomeryForm(power, modEntry);
}

uint64_t montexpeven(uint64_t base, uint64_t exponent, ModEntry modEntry) {
	const uint64_t modulus = modEntry.modulus;
	const uint64_t pwr2 = __builtin_ctzll(modulus);
	const uint64_t modpwr2 = (1ULL << pwr2);
	const uint64_t odd = modulus >> pwr2;
	const ModEntry oddEntry = makeModEntry(odd, modEntry.neginv, modEntry.auxmodsq, modEntry.totient);
	const uint64_t result1 = montexppwr2(base, exponent, modpwr2);
	const uint64_t result2 = montexpodd(base, exponent, oddEntry);
	return semiCRT(result1, makeModEntry(modpwr2, 0, 0, 0), result2, oddEntry, modulus);
}

uint64_t montexp(uint64_t base, uint64_t exponent, ModEntry modEntry) {
	const uint64_t modulus = modEntry.modulus;
	if (__builtin_popcountll(modulus) == 1) {
		// modulus is a power of 2.
		return montexppwr2(base, exponent, modulus);
	}
	if ((modulus & 0b1) == 0) {
		// modulus is an even number but not a power of 2.
		return montexpeven(base,exponent, modEntry);
	}
	// modulus is odd.
	return montexpodd(base, exponent, modEntry);
}

uint64_t invmod(uint64_t residue, ModEntry modEntry) {
	const uint64_t totient = modEntry.totient;
	return montexp(residue, totient - 1, modEntry);
}

int8_t legendre(uint64_t residue, ModEntry modEntry) {
	const uint64_t prime = modEntry.modulus;
	const uint64_t power = montexp(residue, (prime-1)/2, modEntry);
	return (power > 1) ? -1 : power;
}

uint64_t semiCRT(uint64_t res1, ModEntry entry1, uint64_t res2, ModEntry entry2, uint64_t modr) {
	uint64_t mod1 = entry1.modulus;
	uint64_t mod2 = entry2.modulus;
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
		ModEntry tempMod = entry2;
		entry2 = entry1;
		entry1 = tempMod;
	}

	for (uint64_t i = 0; res1 + i * mod1 < modr; i++) {
		if (montmul(res1 + i * mod1, 1, entry2) != res2) {continue;}
		return res1 + i * mod1;
	}
	return 0;
}

ModEntry makeModEntry(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq) {
	return (ModEntry){modulus, neginv, auxmodsq};
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

ModEntry primeModEntry(uint64_t prime) {
	if (prime != 2) {
		return makeModEntry(prime,
			calcNeginv(prime),
			calcAuxmodsq(prime));
	}
	return makeModEntry(2, 0, 0);
}

// We assume the prime of the prime power to be the same as that given as `prime.'
ModEntry increasePrimeModEntryPower(ModEntry powerEntry, ModEntry primeEntry) {
	const uint64_t newModulus = powerEntry.modulus * primeEntry.modulus;
	const uint64_t newNeginv = -powerEntry.neginv * primeEntry.neginv;
	const uint64_t newAuxmodsq = calcAuxmodsq(newModulus);
	return makeModEntry(newModulus, newNeginv, newAuxmodsq);
}

// We assume at most one of the operands to bear an even modulus.
ModEntry combineCoprimeModEntries(ModEntry operand1, ModEntry operand2) {
	const uint64_t modulus1 = operand1.modulus;
	const uint64_t modulus2 = operand2.modulus;
	const uint64_t modulusr = modulus1 * modulus2;

	const uint64_t neginv1 = operand1.neginv;
	const uint64_t neginv2 = operand2.neginv;

	const uint64_t auxmodsq1 = operand1.auxmodsq;
	const uint64_t auxmodsq2 = operand2.auxmodsq;

	if ((modulus1 & 0b1) == 0) {
		return makeModEntry(modulusr, neginv2, auxmodsq2);
	}
	if ((modulus2 & 0b1) == 0) {
		return makeModEntry(modulusr, neginv1, auxmodsq1);
	}

	const uint64_t neginvr = -neginv1 * neginv2;
	const uint64_t auxmodsqr = semiCRT(auxmodsq1, operand1, auxmodsq2, operand2, modulusr);
	return makeModEntry(modulusr, neginvr, auxmodsqr);
}

bool modEntriesEqual(ModEntry operand1, ModEntry operand2) {
	return operand1.modulus == operand2.modulus &&
		operand1.neginv == operand2.neginv &&
		operand1.auxmodsq == operand2.auxmodsq;
}