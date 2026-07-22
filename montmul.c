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

// // We assume at most one of the operands to bear an even modulus.
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