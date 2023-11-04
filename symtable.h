/*********************************************************************/
/* symtable.h                                                        */
/* COS 217 Assignment 3: A Symbol Table ADT                          */
/* Date: 10/31/2023                                                  */
/* Author: Hugh Peterson                                             */
/* Description: A symbol table module to associate string keys with  */
/*              generic values                                       */
/*********************************************************************/

/*********************************************************************/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stdlib.h>

/*
 * A SymTable_T object is a data structure to store key-value pairs, 
 * also known as bindings.
 */
typedef struct SymTable *SymTable_T;

/*********************************************************************/

/*
 * Construct a new SymTable_T. Return NULL if memory is insufficient.
 */
SymTable_T SymTable_new(void);

/*
 * Frees all memory previously allocated for a SymTable_T. 
 * Takes a symbol table oSymTable.
 */
void SymTable_free(SymTable_T oSymTable);

/*
 * Returns a size_t specifying the number of bindings contained within 
 * the specified SymTable_T. Takes a symbol table oSymTable.
 */
size_t SymTable_getLength(SymTable_T oSymTable);

/*
 * Tries to insert a new key-value binding with a String key and 
 * generic value into the specified SymTable_T. Returns 1 if successful
 * and 0 if binding is already present or memory is insufficient.
 * Takes a symbol table oSymTable, a key pcKey, and a value pvValue.
 */
int SymTable_put(SymTable_T oSymTable,
     const char *pcKey, const void *pvValue);

/*
 * If *pcKey is present as a key, its value is changed to *pcValue and 
 * the old value is returned. Otherwise, NULL is returned.
 * Takes a symbol table oSymTable, a key pcKey, and a value pvValue.
 */
void *SymTable_replace(SymTable_T oSymTable,
     const char *pcKey, const void *pvValue);

/*
 * Returns 1 if pcKey is present in oSymTable and 0 otherwise. 
 * Takes a symbol table oSymTable and a key pcKey.
 */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*
 * If pcKey is present in oSymTable, returns its associated value. 
 * Returns NULL otherwise.
 */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*
 * If pcKey is present in oSymTable, removes its binding and returns 
 * the associated value. Returns NULL otherwise.
 */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*
 * Applies (*pfApply) to all bindings in oSymTable, passing 
 * *pvExtra as a parameter. pfApply takes a key pcKey, a value 
 * pvValue, and an extra parameter pvExtra.
 */
void SymTable_map(SymTable_T oSymTable,
     void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
     const void *pvExtra);

/*********************************************************************/

#endif

/*********************************************************************/
