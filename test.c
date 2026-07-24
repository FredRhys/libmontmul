#include "montmul.h"
#include <assert.h>
#include <stdio.h>


void testArithmetic(void) {
    // Odd prime moduli
    {
        ModEntry modEntry = makeModEntry(5, 3689348814741910323, 1, 4);
        assert(montmul(3, 4, modEntry) == 2);
        assert(montmul(3, 5, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 0);
        assert(montmul(1234, 6789, modEntry) == 1);
    }{
        ModEntry modEntry = makeModEntry(13, 12770822820260458811ULL, 9, 12);
        assert(montmul(3, 12, modEntry) == 10);
        assert(montmul(3, 13, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 6);
        assert(montmul(1234, 6789, modEntry) == 10);
    }

    // Power-of-two moduli
    {
        ModEntry modEntry = makeModEntry(2, 0, 0, 1);
        assert(montmul(3, 1, modEntry) == 1);
        assert(montmul(3, 2, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 0);
        assert(montmul(1234, 6789, modEntry) == 0);
    }{
        ModEntry modEntry = makeModEntry(16, 0, 0, 8);
        assert(montmul(3, 15, modEntry) == 13);
        assert(montmul(3, 16, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 2);
        assert(montmul(1234, 6789, modEntry) == 10);
    }

    // Composite moduli
    {
        ModEntry modEntry = makeModEntry(10, 3689348814741910323, 1, 4);
        assert(montmul(3, 9, modEntry) == 7);
        assert(montmul(3, 10, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 0);
        assert(montmul(1234, 6789, modEntry) == 6);
    }{
        ModEntry modEntry = makeModEntry(35, 5797548137451573365, 11, 24);
        assert(montmul(3, 34, modEntry) == 32);
        assert(montmul(3, 35, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 30);
        assert(montmul(1234, 6789, modEntry) == 26);
    }
}

void testEntriesEqual(void) {
    ModEntry modEntry = makeModEntry(5, 3689348814741910323, 1, 4);
    assert(modEntriesEqual(modEntry, modEntry));
    assert(!modEntriesEqual(modEntry, makeModEntry(5, 3689348814741910323, 2, 4)));
}

void testCombo(void) {
    {
        ModEntry entry1 = makeModEntry(5, 3689348814741910323, 1, 4);
        ModEntry entry2 = makeModEntry(13, 12770822820260458811ULL, 9, 12);
        ModEntry expected = makeModEntry(65, 17311559823019733055ULL, 61, 48);
        ModEntry actual = combineCoprimeModEntries(entry1, entry2);
        assert(modEntriesEqual(actual, expected));
    }{
        ModEntry entry1 = makeModEntry(5, 3689348814741910323, 1, 4);
        ModEntry entry2 = makeModEntry(2, 0, 0, 1);
        ModEntry expected = makeModEntry(10, 3689348814741910323, 1, 4);
        ModEntry actual = combineCoprimeModEntries(entry1, entry2);
        assert(modEntriesEqual(actual, expected));
    }{
        ModEntry entry1 = makeModEntry(5, 3689348814741910323, 1, 4);
        ModEntry entry2 = makeModEntry(2, 0, 0, 1);
        ModEntry expected = makeModEntry(10, 3689348814741910323, 1, 4);
        ModEntry actual = combineCoprimeModEntries(entry2, entry1);
        assert(modEntriesEqual(actual, expected));
    }
}

void testMakePrime(void) {
    {
        ModEntry expected = makeModEntry(5, 3689348814741910323, 1, 4);
        ModEntry actual = primeModEntry(5);
        assert(modEntriesEqual(actual, expected));
    }
    {
        ModEntry expected = makeModEntry(13, 12770822820260458811ULL, 9, 12);
        ModEntry actual = primeModEntry(13);
        assert(modEntriesEqual(actual, expected));
    }
    {
        ModEntry expected = makeModEntry(2, 0, 0, 1);
        ModEntry actual = primeModEntry(2);
        assert(modEntriesEqual(actual, expected));
    }
}

void testIncreasePrimePower(void) {
    {
        ModEntry prime = makeModEntry(5, 3689348814741910323, 1, 4);
        ModEntry expected1 = makeModEntry(25, 8116567392432202711, 6, 20);
        ModEntry actual1 = increasePrimeModEntryPower(prime, prime);
        assert(modEntriesEqual(actual1, expected1));
        ModEntry expected2 = makeModEntry(125, 16380708737454081835ULL, 81, 100);
        ModEntry actual2 = increasePrimeModEntryPower(actual1, prime);
        assert(modEntriesEqual(actual2, expected2));
    }
    {
        ModEntry prime = makeModEntry(13, 12770822820260458811ULL, 9, 12);
        ModEntry expected1 = makeModEntry(169, 982370986173881447, 48, 156);
        ModEntry actual1 = increasePrimeModEntryPower(prime, prime);
        assert(modEntriesEqual(actual1, expected1));
        ModEntry expected2 = makeModEntry(2197, 75566998936452419, 217, 2028);
        ModEntry actual2 = increasePrimeModEntryPower(actual1, prime);
        assert(modEntriesEqual(actual2, expected2));
    }
}

void testExp(void) {
    {
        ModEntry modEntry = makeModEntry(5, 3689348814741910323, 1, 4);
        assert(montexp(2, 3, modEntry) == 3);
        assert(montexp(2, 4, modEntry) == 1);
        assert(montexp(3, 4, modEntry) == 1);
        assert(montexp(4, 4, modEntry) == 1);
    }{
        ModEntry modEntry = makeModEntry(10, 3689348814741910323, 1, 4);
        assert(montexp(2, 3, modEntry) == 8);
        assert(montexp(3, 4, modEntry) == 1);
        assert(montexp(7, 4, modEntry) == 1);
        assert(montexp(9, 4, modEntry) == 1);
    }{
        ModEntry modEntry = makeModEntry(16, 3689348814741910323, 1, 4);
        assert(montexp(2, 3, modEntry) == 8);
        assert(montexp(2, 4, modEntry) == 0);
        assert(montexp(3, 8, modEntry) == 1);
        assert(montexp(5, 8, modEntry) == 1);
    }
}

void testNT(void) {
    {
        ModEntry modEntry = makeModEntry(5, 3689348814741910323, 1, 4);
        assert(legendre(4, modEntry) == 1);
        assert(legendre(2, modEntry) == -1);
        assert(legendre(5, modEntry) == 0);

        assert(invmod(4, modEntry) == 4);
        assert(invmod(2, modEntry) == 3);
        assert(invmod(1, modEntry) == 1);
        assert(invmod(3, modEntry) == 2);
        assert(invmod(0, modEntry) == 0);
    }
}

int main(void) {
    (void)testArithmetic();
    (void)testEntriesEqual();
    (void)testCombo();
    (void)testIncreasePrimePower();
    (void)testExp();
    (void)testNT();
    return 0;
}