#ifndef MONTMUL_H

#define MONTMUL_H

typedef struct ModEntry{
	uint64_t modulus;
	uint64_t neginv;
	uint64_t auxmodsq;
}ModEntry;

ModEntry makeModEntry(uint64_t modulus, uint64_t neginv, uint64_t auxmodsq);
ModEntry combineModEntries(ModEntry operand1, ModEntry operand2);
uint64_t montmul(uint64_t multiplier, uint64_t multiplicand, ModEntry modEntry);

#endif