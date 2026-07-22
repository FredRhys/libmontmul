#ifndef MONTMUL_H

#define MONTMUL_H
#include <stdbit.h>
#include <stdint.h>


typedef struct ModEntry{
	uint64_t modulus;
	uint64_t neginv;
	uint64_t auxmodsq;
}ModEntry;

ModEntry makeModEntry(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq);
// ModEntry combineCoprimeModEntries(ModEntry operand1, ModEntry operand2);
// ModEntry increasePrimeModEntryPower(ModEntry operand, uint64_t prime);
uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry);

#endif