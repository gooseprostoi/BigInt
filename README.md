# BigInt
BigInt (3sem)

Основная идея - хранить "разряды" в векторе, например, {1,3,0,213} <-> 1 + 3 * 2^(32) + 0 * 2^(64) + 213 * 2^(96).

Сначала был реализован BigUInt (натуральные числа с нулём), а потом на нём уже BigInt.

В Test.cpp есть немного тестов (я почти полностью уверен, что их нужно писать по-другому). Test.out - скомпиленный файл тестов.