#ifndef MONTMUL_H

#define MONTMUL_H
#include <stdbit.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct ModEntry{
	uint64_t modulus;
	uint64_t neginv;
	uint64_t auxmodsq;
	uint64_t totient;
}ModEntry;

ModEntry makeModEntry(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq, uint64_t totient);
ModEntry primeModEntry(uint64_t prime);
ModEntry combineCoprimeModEntries(ModEntry operand1, ModEntry operand2);
ModEntry increasePrimeModEntryPower(ModEntry powerEntry, ModEntry primeEntry);
uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry);
uint64_t intermul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry);
uint64_t montexp(uint64_t base, uint64_t exponent, ModEntry modEntry);
bool modEntriesEqual(ModEntry operand1, ModEntry operand2);

#endif