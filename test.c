#include "montmul.h"
#include <assert.h>
#include <stdio.h>


void testArithmetic(void) {
    // Odd prime moduli
    {
        ModEntry modEntry = makeModEntry(5, 3689348814741910323, 1);
        assert(montmul(3, 4, modEntry) == 2);
        assert(montmul(3, 5, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 0);
        assert(montmul(1234, 6789, modEntry) == 1);
    }{
        ModEntry modEntry = makeModEntry(13, 12770822820260458811ULL, 9);
        assert(montmul(3, 12, modEntry) == 10);
        assert(montmul(3, 13, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 6);
        assert(montmul(1234, 6789, modEntry) == 10);
    }

    // Power-of-two moduli
    {
        ModEntry modEntry = makeModEntry(2, 0, 0);
        assert(montmul(3, 1, modEntry) == 1);
        assert(montmul(3, 2, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 0);
        assert(montmul(1234, 6789, modEntry) == 0);
    }{
        ModEntry modEntry = makeModEntry(16, 0, 0);
        assert(montmul(3, 15, modEntry) == 13);
        assert(montmul(3, 16, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 2);
        assert(montmul(1234, 6789, modEntry) == 10);
    }

    // Composite moduli
    {
        ModEntry modEntry = makeModEntry(10, 3689348814741910323, 1);
        assert(montmul(3, 9, modEntry) == 7);
        assert(montmul(3, 10, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 0);
        assert(montmul(1234, 6789, modEntry) == 6);
    }{
        ModEntry modEntry = makeModEntry(35, 5797548137451573365, 11);
        assert(montmul(3, 34, modEntry) == 32);
        assert(montmul(3, 35, modEntry) == 0);
        assert(montmul(12345, 67890, modEntry) == 30);
        assert(montmul(1234, 6789, modEntry) == 26);
    }
}

void testEntriesEqual(void) {
    ModEntry modEntry = makeModEntry(5, 3689348814741910323, 1);
    assert(modEntriesEqual(modEntry, modEntry));
    assert(!modEntriesEqual(modEntry, (ModEntry){5, 3689348814741910323, 2}));
}

void testCombo(void) {
    {
        ModEntry entry1 = makeModEntry(5, 3689348814741910323, 1);
        ModEntry entry2 = makeModEntry(13, 12770822820260458811ULL, 9);
        ModEntry expected = makeModEntry(65, 17311559823019733055ULL, 61);
        ModEntry actual = combineCoprimeModEntries(entry1, entry2);
        assert(modEntriesEqual(actual, expected));
    }{
        ModEntry entry1 = makeModEntry(5, 3689348814741910323, 1);
        ModEntry entry2 = makeModEntry(2, 0, 0);
        ModEntry expected = makeModEntry(10, 3689348814741910323, 1);
        ModEntry actual = combineCoprimeModEntries(entry1, entry2);
        assert(modEntriesEqual(actual, expected));
    }{
        ModEntry entry1 = makeModEntry(5, 3689348814741910323, 1);
        ModEntry entry2 = makeModEntry(2, 0, 0);
        ModEntry expected = makeModEntry(10, 3689348814741910323, 1);
        ModEntry actual = combineCoprimeModEntries(entry2, entry1);
        assert(modEntriesEqual(actual, expected));
    }
}

int main(void) {
    (void)testArithmetic();
    (void)testEntriesEqual();
    (void)testCombo();
    return 0;
}