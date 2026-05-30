#include <stdint.h>
#include <stdbit.h>

typedef struct ModEntry{
	uint64_t modulus;
	uint64_t neginv;
	uint64_t auxmodsq;
}ModEntry;

ModEntry makeModEntry(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq) {
	return (ModEntry){modulus, neginv, auxmodsq};
}

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

uint64_t findresultpwr2(uint64_t result1, uint64_t result2, uint64_t pwr2, ModEntry oddEntry, uint64_t modulus) {
	uint64_t result = result1;
	while (result < modulus) {
		if (montmulodd(result, 1, oddEntry) == result2) {
			return result;
		}
		result += pwr2;
	}
	return 0; // error
}

uint64_t findresultodd(uint64_t result1, uint64_t result2, uint64_t pwr2, uint64_t odd, uint64_t modulus) {
	uint64_t result = result2;
	while (result < modulus) {
		if (montmulpwr2(result, 1, pwr2) == result1) {
			return result;
		}
		result += odd;
	}
	return 0; // error
}

uint64_t findresult(uint64_t result1, uint64_t result2, uint64_t pwr2, ModEntry oddEntry, uint64_t modulus) {
	const uint64_t odd = oddEntry.modulus;
	if (pwr2 >= odd) {
		return findresultpwr2(result1, result2, pwr2, oddEntry, modulus);
	}
	else {
		return findresultodd(result1, result2, pwr2, odd, modulus);
	}
}

uint64_t montmuleven(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry) {
	const uint64_t modulus = modEntry.modulus;
	const uint64_t pwr2 = __builtin_ctzll(modulus);
	const uint64_t odd = modulus >> pwr2;
	const ModEntry oddEntry = makeModEntry(odd, modEntry.neginv, modEntry.auxmodsq);
	const uint64_t result1 = montmulpwr2(multiplier, multiplicand, 1LL << pwr2);
	const uint64_t result2 = montmulodd(multiplier, multiplicand, oddEntry);
	return findresult(result1, result2, pwr2, oddEntry, modulus);
}

uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry) {
	const uint64_t modulus = modEntry.modulus;
	if (__builtin_popcountll(modulus) == 1) {
		return montmulpwr2(multiplier, multiplicand, modulus);
	}
	else if ((modulus & 0b1) == 0) {
		return montmuleven(multiplier, multiplicand, modEntry);
	}
	else {
		return montmulodd(multiplier, multiplicand, modEntry);
	}
}

int main(void) {
	return 0;
}