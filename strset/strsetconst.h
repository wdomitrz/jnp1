/*
 * Authors:
 *  Damian Obara
 *  Piotr Ogonowski
 *  Witalis Domitrz
 */

#ifndef STRSET_STRSETCONST_H
#define STRSET_STRSETCONST_H

// Zainkludowanie iostream-a, żeby std::cerr był zainicjalizowany
#ifdef __cplusplus
#include<iostream>
#endif //__cplusplus

// Zapewnienie kompatybilności z C i utworzenie namespace
#ifdef __cplusplus
extern "C" {
namespace jnp1 {
#endif //__cplusplus

	/*
	 * Zwraca identyfikator zbioru, którego nie można modyfikować i który zawiera
	 * jeden element: napis "42". Zbiór jest tworzony przy pierwszym wywołaniu tej
	 * funkcji i wtedy zostaje ustalony jego numer.
	 */
	unsigned long strset42();

#ifdef __cplusplus
} //end namespace jnp1
} //end extern "C"
#endif //__cplusplus

#endif //STRSET_STRSETCONST_H
