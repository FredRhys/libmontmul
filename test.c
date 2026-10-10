#include "montmul.h"
#include <assert.h>
#include <stdio.h>

void testAddSub(void) {
    assert(addmod(3, 4, 5) == 2);
    assert(submod(3, 4, 5) == 4);
}

void testArithmetic(void) {
    // Odd prime moduli
    {
        const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        assert(montmul(3, 4, FIVE) == 2);
        assert(montmul(3, 5, FIVE) == 0);
        assert(montmul(12345, 67890, FIVE) == 0);
        assert(montmul(1234, 6789, FIVE) == 1);
    }{
        const struct libmontmul_t THIRTEEN = makeLibmontmul_t(13, 12770822820260458811ULL, 9, 12);
        assert(montmul(3, 12, THIRTEEN) == 10);
        assert(montmul(3, 13, THIRTEEN) == 0);
        assert(montmul(12345, 67890, THIRTEEN) == 6);
        assert(montmul(1234, 6789, THIRTEEN) == 10);
    }

    // Power-of-two moduli
    {
        const struct libmontmul_t TWO = makeLibmontmul_t(2, 0, 0, 1);
        assert(montmul(3, 1, TWO) == 1);
        assert(montmul(3, 2, TWO) == 0);
        assert(montmul(12345, 67890, TWO) == 0);
        assert(montmul(1234, 6789, TWO) == 0);
    }{
        const struct libmontmul_t SIXTEEN = makeLibmontmul_t(16, 0, 0, 8);
        assert(montmul(3, 15, SIXTEEN) == 13);
        assert(montmul(3, 16, SIXTEEN) == 0);
        assert(montmul(12345, 67890, SIXTEEN) == 2);
        assert(montmul(1234, 6789, SIXTEEN) == 10);
    }

    // Composite moduli
    {
        const struct libmontmul_t TEN = makeLibmontmul_t(10, 3689348814741910323, 1, 4);
        assert(montmul(3, 9, TEN) == 7);
        assert(montmul(3, 10, TEN) == 0);
        assert(montmul(12345, 67890, TEN) == 0);
        assert(montmul(1234, 6789, TEN) == 6);
    }{
        const struct libmontmul_t THIRTY_FIVE = makeLibmontmul_t(35, 5797548137451573365, 11, 24);
        assert(montmul(3, 34, THIRTY_FIVE) == 32);
        assert(montmul(3, 35, THIRTY_FIVE) == 0);
        assert(montmul(12345, 67890, THIRTY_FIVE) == 30);
        assert(montmul(1234, 6789, THIRTY_FIVE) == 26);
    }
}

void testEntriesEqual(void) {
    const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
    assert(equalsLibmontmul_t(FIVE, FIVE));
    assert(!equalsLibmontmul_t(FIVE, makeLibmontmul_t(5, 3689348814741910323, 2, 4)));
}

void testCombo(void) {
    {
        const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        const struct libmontmul_t THIRTEEN = makeLibmontmul_t(13, 12770822820260458811ULL, 9, 12);
        const struct libmontmul_t EXPECTED = makeLibmontmul_t(65, 17311559823019733055ULL, 61, 48);
        const struct libmontmul_t ACTUAL = compositeLibmontmul_t(FIVE, THIRTEEN);
        assert(equalsLibmontmul_t(EXPECTED, ACTUAL));
    }{
        const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        const struct libmontmul_t TWO = makeLibmontmul_t(2, 0, 0, 1);
        const struct libmontmul_t EXPECTED = makeLibmontmul_t(10, 3689348814741910323, 1, 4);
        const struct libmontmul_t ACTUAL = compositeLibmontmul_t(FIVE, TWO);
        assert(equalsLibmontmul_t(EXPECTED, ACTUAL));
    }{
        const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        const struct libmontmul_t TWO = makeLibmontmul_t(2, 0, 0, 1);
        const struct libmontmul_t EXPECTED = makeLibmontmul_t(10, 3689348814741910323, 1, 4);
        const struct libmontmul_t ACTUAL = compositeLibmontmul_t(TWO, FIVE);
        assert(equalsLibmontmul_t(EXPECTED, ACTUAL));
    }
}

void testMakePrime(void) {
    {
        const struct libmontmul_t EXPECTED = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        const struct libmontmul_t ACTUAL = primeLibmontmul_t(5);
        assert(equalsLibmontmul_t(EXPECTED, ACTUAL));
    }
    {
        const struct libmontmul_t EXPECTED = makeLibmontmul_t(13, 12770822820260458811ULL, 9, 12);
        const struct libmontmul_t ACTUAL = primeLibmontmul_t(13);
        assert(equalsLibmontmul_t(EXPECTED, ACTUAL));
    }
    {
        const struct libmontmul_t EXPECTED = makeLibmontmul_t(2, 0, 0, 1);
        const struct libmontmul_t ACTUAL = primeLibmontmul_t(2);
        assert(equalsLibmontmul_t(EXPECTED, ACTUAL));
    }
}

void testIncreasePrimePower(void) {
    {
        const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        const struct libmontmul_t EXPECTED1 = makeLibmontmul_t(25, 8116567392432202711, 6, 20);
        const struct libmontmul_t ACTUAL1 = powerLibmontmul_t(FIVE, FIVE);
        assert(equalsLibmontmul_t(ACTUAL1, EXPECTED1));
        const struct libmontmul_t EXPECTED2 = makeLibmontmul_t(125, 16380708737454081835ULL, 81, 100);
        const struct libmontmul_t ACTUAL2 = powerLibmontmul_t(ACTUAL1, FIVE);
        assert(equalsLibmontmul_t(ACTUAL2, EXPECTED2));
    }
    {
        const struct libmontmul_t THIRTEEN = makeLibmontmul_t(13, 12770822820260458811ULL, 9, 12);
        const struct libmontmul_t EXPECTED1 = makeLibmontmul_t(169, 982370986173881447, 48, 156);
        const struct libmontmul_t ACTUAL1 = powerLibmontmul_t(THIRTEEN, THIRTEEN);
        assert(equalsLibmontmul_t(ACTUAL1, EXPECTED1));
        const struct libmontmul_t EXPECTED2 = makeLibmontmul_t(2197, 75566998936452419, 217, 2028);
        const struct libmontmul_t ACTUAL2 = powerLibmontmul_t(ACTUAL1, THIRTEEN);
        assert(equalsLibmontmul_t(ACTUAL2, EXPECTED2));
    }
}

void testExp(void) {
    {
        const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        assert(montexp(2, 3, FIVE) == 3);
        assert(montexp(2, 4, FIVE) == 1);
        assert(montexp(3, 4, FIVE) == 1);
        assert(montexp(4, 4, FIVE) == 1);
    }{
        const struct libmontmul_t TEN = makeLibmontmul_t(10, 3689348814741910323, 1, 4);
        assert(montexp(2, 3, TEN) == 8);
        assert(montexp(3, 4, TEN) == 1);
        assert(montexp(7, 4, TEN) == 1);
        assert(montexp(9, 4, TEN) == 1);
    }{
        const struct libmontmul_t SIXTEEN = makeLibmontmul_t(16, 3689348814741910323, 1, 4);
        assert(montexp(2, 3, SIXTEEN) == 8);
        assert(montexp(2, 4, SIXTEEN) == 0);
        assert(montexp(3, 8, SIXTEEN) == 1);
        assert(montexp(5, 8, SIXTEEN) == 1);
    }
}

void testNT(void) {
    {
        const struct libmontmul_t FIVE = makeLibmontmul_t(5, 3689348814741910323, 1, 4);
        assert(legendre(4, FIVE) == 1);
        assert(legendre(2, FIVE) == -1);
        assert(legendre(5, FIVE) == 0);

        assert(invmod(4, FIVE) == 4);
        assert(invmod(2, FIVE) == 3);
        assert(invmod(1, FIVE) == 1);
        assert(invmod(3, FIVE) == 2);
        assert(invmod(0, FIVE) == 0);
    }
}

void testSqrt(void) {
    {
        const struct libmontmul_t SEVEN = makeLibmontmul_t(7, 10540996613548315209ULL, 4, 6);
        assert(montexp(sqrtmod(1, SEVEN), 2, SEVEN) == 1);
        assert(montexp(sqrtmod(2, SEVEN), 2, SEVEN) == 2);
        assert(montexp(sqrtmod(4, SEVEN), 2, SEVEN) == 4);
        assert(montexp(sqrtmod(9, SEVEN), 2, SEVEN) == 2);
    }
    {
        const struct libmontmul_t THIRTEEN = makeLibmontmul_t(13, 12770822820260458811ULL, 9, 12);
        assert(montexp(sqrtmod(1, THIRTEEN), 2, THIRTEEN) == 1);
        assert(montexp(sqrtmod(3, THIRTEEN), 2, THIRTEEN) == 3);
        assert(montexp(sqrtmod(4, THIRTEEN), 2, THIRTEEN) == 4);
        assert(montexp(sqrtmod(9, THIRTEEN), 2, THIRTEEN) == 9);
        assert(montexp(sqrtmod(10, THIRTEEN), 2, THIRTEEN) == 10);
        assert(montexp(sqrtmod(12, THIRTEEN), 2, THIRTEEN) == 12);
    }
}

void testCRT() {
    {
        const struct libmontmul_t SEVEN = makeLibmontmul_t(7, 10540996613548315209ULL, 4, 6);
        const struct libmontmul_t THIRTEEN = makeLibmontmul_t(13, 12770822820260458811ULL, 9, 12);
        const struct libmontmul_t NINETY_ONE = makeLibmontmul_t(91, 15000649026972602413ULL, 74, 72);
        assert(crtCalc(0, SEVEN, 0, THIRTEEN, NINETY_ONE) == 0);
        assert(crtCalc(0, SEVEN, 1, THIRTEEN, NINETY_ONE) == 14);
        assert(crtCalc(1, SEVEN, 0, THIRTEEN, NINETY_ONE) == 78);
        assert(crtCalc(4, SEVEN, 5, THIRTEEN, NINETY_ONE) == 18);
    }
}

int main(void) {
    (void)testAddSub();
    (void)testMakePrime();
    (void)testArithmetic();
    (void)testEntriesEqual();
    (void)testCombo();
    (void)testIncreasePrimePower();
    (void)testExp();
    (void)testNT();
    (void)testSqrt();
    (void)testCRT();
    return 0;
}