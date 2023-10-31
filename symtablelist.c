/*********************************************************************/
/* symtable.c                                                        */
/* COS 217 Assignment 3: A Symbol Table ADT                          */
/* Date: 10/31/2023                                                  */
/* Author: Hugh Peterson                                             */
/* Description: A symbol table module to associate string keys with  */
/*              generic values                                       */
/*********************************************************************/

/*********************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/*********************************************************************/

/*
 * Stores a key-value pair and a pointer to the next Binding.
 */
struct Binding {
   /* key */
   char *key;

   /* value */
   void *val;

   /* pointer to next Binding */
   struct Binding *next;
};

/*
 * Structure storing size and a pointer to the first Binding.
 */
struct SymTable {
   /* pointer to first Binding */
   struct Binding *first;

   /* number of bindings stored in the SymTable */
   size_t size;
};
      

/*********************************************************************/

/*
 * Construct a new SymTable_T. Return NULL if memory is insufficient.
 */
SymTable_T SymTable_new(void) {
   SymTable_T st;

   st = (SymTable_T) malloc(sizeof(struct SymTable));
   if (st == NULL) {
      return NULL;
   }

   st -> first = NULL;
   st -> size = 0;
   
   return st;
}

/*
 * Frees all memory previously allocated for a SymTable_T
 */
void SymTable_free(SymTable_T oSymTable) {
   struct Binding *previous;
   struct Binding *current;

   assert(oSymTable != NULL);

   current = oSymTable->first;
   while (current != NULL) {
      previous = current;
      current = current->next;

      free(previous);
   }

   free(oSymTable);
}

/*
 * Returns a size_t specifying the number of bindings contained within 
 * the specified SymTable_T.
 */
size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);
   
   return oSymTable->size;
}

/*
 * Tries to insert a new key-value binding with a String key and 
 * generic value into the specified SymTable_T. Returns 1 if successful
 * and 0 if binding is already present or memory is insufficient.
 */
int SymTable_put(SymTable_T oSymTable,
                 const char *pcKey, const void *pvValue) {
   struct Binding *nextBind;
   struct Binding *newBind;
   char *keyCopy;
   int keyLen;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   if (SymTable_contains(oSymTable, pcKey)) {
      return 0;
   }

   newBind = (struct Binding*)malloc(sizeof(struct Binding));
   if (newBind == NULL) {
      return 0;
   }

   /* Duplicate key */
   keyLen = (int) strlen(pcKey);
   keyCopy = (char *) malloc(keyLen + 1);
   if (keyCopy == NULL) {
      return 0;
   }
   strcpy(keyCopy, pcKey);
   
   
   /* make temp pointer to first */
   nextBind = oSymTable->first;

   /* Assign values to new Node */
   oSymTable->first = newBind;
   newBind->val = (void *) pvValue;
   newBind->next = nextBind;
   newBind->key = keyCopy;

   oSymTable->size++;
   return 1;
}

/*
 * If *pcKey is present as a key, its value is changed to *pcValue and 
 * the old value is returned. Otherwise, NULL is returned.
 */
void *SymTable_replace(SymTable_T oSymTable,
                       const char *pcKey, const void *pvValue) {
   struct Binding *current;
   void *oldVal;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* Find and replace binding */
   current = oSymTable->first;
   while (current != NULL) {
      if (strcmp(pcKey, current->key) == 0) {
         /* change value */
         oldVal = current->val;
         current->val = (void *) pvValue;

         return oldVal;
      }
      current = current->next;
   }

   return NULL;
}

/*
 * Returns 1 if pcKey is present and 0 otherwise.
 */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
   struct Binding *current;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   current = oSymTable->first;
   while (current != NULL) {
      if (strcmp(pcKey, current->key) == 0) {
         return 1;
      }
      current = current->next;
   }

   return 0;
}

/*
 * If pcKey is present, returns its associated value. Returns NULL 
 * otherwise.
 */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
   struct Binding *current;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   current = oSymTable->first;
   while (current != NULL) {
      if (strcmp(pcKey, current->key) == 0) {
         return current->val;
      }
      current = current->next;
   }

   return NULL;
}

/*
 * If pcKey is present, removes its binding and returns the associated 
 * value. Returns NULL otherwise.
 */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
   struct Binding *previous;
   struct Binding *current;
   void *removedValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   if (!SymTable_contains(oSymTable, pcKey)) {
      return NULL;
   }
   
   /* Handle empty */
   if (!oSymTable->first) {
      /*DEBUG*/
      #include <stdio.h>
      printf("empty rm\n");
      return NULL;
   }
   /* Handle removal of first */
   if ((SymTable_getLength(oSymTable) == 1) ||
       (strcmp(pcKey, oSymTable->first->key) == 0)) {

      removedValue = oSymTable->first->val;
      current = oSymTable->first->next;
      
      oSymTable->size--;
      free(oSymTable->first);
      
      oSymTable->first = current;

      return removedValue;
   }

   /* Handle other */
   previous = oSymTable->first;
   current = oSymTable->first->next;
   do {
      if (strcmp(pcKey, current->key) == 0) {
         removedValue = current->val;

         oSymTable->size--;
         previous->next = current->next;
         free(current);
         current = NULL;

         return removedValue;
      }
      previous = current;
      current = current->next;
   } while (current != NULL);

   return NULL;
}

/*
 * Applies (*pfApply) to all bindings in the symbol table, passing 
 * *pvExtra as a parameter.
 */
void SymTable_map(SymTable_T oSymTable,
     void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra) {

   struct Binding *current;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   
   current = oSymTable->first;
   while (current != NULL) {

      (*pfApply)((current->key), (current->val), (void *) pvExtra);
      
      current = current->next;
   }
}

/*********************************************************************/

#ifdef DEBUG

/*********************************************************************/

#include <stdio.h>

void scale(const char *key, void *val, void *alpha) {
   *(int*)val = *(int*)val * *(int*)alpha;
}

int main(void) {
   SymTable_T test = SymTable_new();
   SymTable_T empty = SymTable_new();
   int vals[11] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 69};

   if (test != NULL) {
      printf("Successfully created SymTable!\n");
   }

/*********************************************************************/

   /* TEST LENGTH AND PUT */
   printf("\nTESTING LENGTH AND PUT\n\n");
   
   /* 0 */
   printf("Initial length = %d\n", (int) SymTable_getLength(test));

   SymTable_put(test, "a", vals + 0);
   SymTable_put(test, "b", vals + 1);
   SymTable_put(test, "c", vals + 2);
   SymTable_put(test, "d", vals + 3);
   SymTable_put(test, "e", vals + 4);
   SymTable_put(test, "f", vals + 5);

   /* 1 */
   printf("Length after putting 6 bindings = %d\n",
          (int) SymTable_getLength(test));

/*********************************************************************/

   /* TEST CONTAINS */
   printf("\nTESTING CONTAINS\n\n");
   
   /* 1 */
   printf("ST contains key \"a\"? %d\n", SymTable_contains(test, "a"));
   /* 1 */
   printf("ST contains key \"d\"? %d\n", SymTable_contains(test, "d"));
   /* 1 */
   printf("ST contains key \"f\"? %d\n", SymTable_contains(test, "f"));
   /* 0 */
   printf("ST contains key \"NO\"? %d\n",
          SymTable_contains(test, "NO"));
   /* 0 */
   printf("Empty ST contains key \"NO\"? %d\n",
          SymTable_contains(empty, "NO"));

/*********************************************************************/

   /* TEST GET */
   printf("\nTESTING GET\n\n");
   
   /* 1 */
   printf("Value of key \"a\" = %d\n", *(int*)SymTable_get(test, "a"));
   /* 4 */
   printf("Value of key \"d\" = %d\n", *(int*)SymTable_get(test, "d"));
   /* 9 */
   printf("Value of key \"f\" = %d\n", *(int*)SymTable_get(test, "f"));
   /* 1 */
   printf("Value of key \"NO\" == NULL? %d\n",
      (SymTable_get(test, "NO") == 0));
   /* 1 */
   printf("Value of key \"NO\" in empty ST == NULL? %d\n",
          (SymTable_get(empty, "NO") == 0));

/*********************************************************************/

   /* TEST REPLACE */
   printf("\nTESTING REPLACE\n\n");

   /* 4 */
   printf("Old value of key \"d\" = %d\n",
          *(int*)SymTable_replace(test, "d", vals + 10));
   /* 69 */
   printf("New value of key \"d\" = %d\n",
          *(int*)SymTable_get(test, "d"));

/*********************************************************************/

   /* TEST MAP */
   printf("\nTESTING MAP\n\n");

   printf("Old value of key \"a\" = %d\n",
          *(int*)SymTable_get(test, "a"));
   printf("Old value of key \"b\" = %d\n",
          *(int*)SymTable_get(test, "b"));
   printf("Old value of key \"c\" = %d\n",
          *(int*)SymTable_get(test, "c"));
   printf("Old value of key \"d\" = %d\n",
          *(int*)SymTable_get(test, "d"));
   printf("Old value of key \"e\" = %d\n",
          *(int*)SymTable_get(test, "e"));
   printf("Old value of key \"f\" = %d\n",
          *(int*)SymTable_get(test, "f"));
   
   SymTable_map(test, scale, vals + 2);

   printf("New value of key \"a\" = %d\n",
          *(int*)SymTable_get(test, "a"));
   printf("New value of key \"b\" = %d\n",
          *(int*)SymTable_get(test, "b"));
   printf("New value of key \"c\" = %d\n",
          *(int*)SymTable_get(test, "c"));
   printf("New value of key \"d\" = %d\n",
          *(int*)SymTable_get(test, "d"));
   printf("New value of key \"e\" = %d\n",
          *(int*)SymTable_get(test, "e"));
   printf("New value of key \"f\" = %d\n",
          *(int*)SymTable_get(test, "f"));

/*********************************************************************/
   
   /* TEST REMOVE */
   printf("\nTESTING REMOVE\n\n");
   
   /* 4, 5 */
   printf("Removed (d, %d). Size = %d\n",
          *(int*)SymTable_remove(test, "d"),
          (int)SymTable_getLength(test));
   /* 6, 4 */
   printf("Removed (f, %d). Size = %d\n",
          *(int*)SymTable_remove(test, "f"),
          (int)SymTable_getLength(test));
   /* 1, 3 */
   printf("Removed (a, %d). Size = %d\n",
          *(int*)SymTable_remove(test, "a"),
          (int)SymTable_getLength(test));

   /* 1 */
   printf("Remove nonexistent == NULL? %d\n",
          ((int*)SymTable_remove(test, "NO") == 0));

   /* 1 */
   printf("Remove previously existent == NULL? %d\n",
          ((int*)SymTable_remove(test, "a") == 0));
   
   /* 2, 2 */
   printf("Removed (b, %d). Size = %d\n",
          *(int*)SymTable_remove(test, "b"),
          (int)SymTable_getLength(test));
   /* 3, 1 */
   printf("Removed (c, %d). Size = %d\n",
          *(int*)SymTable_remove(test, "c"),
          (int)SymTable_getLength(test));
   /* 5, 0 */
   printf("Removed (e, %d). Size = %d\n",
          *(int*)SymTable_remove(test, "e"),
          (int)SymTable_getLength(test));

   /* 1 */
   printf("Remove too many == NULL? %d\n",
          ((int*)SymTable_remove(test, "f") == 0));

/*********************************************************************/
   
   return 0;
}

/*********************************************************************/

#endif

/*********************************************************************/
