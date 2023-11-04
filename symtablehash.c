/*********************************************************************/
/* symtablehash.c                                                    */
/* COS 217 Assignment 3: A Symbol Table ADT                          */
/* Date: 10/31/2023                                                  */
/* Author: Hugh Peterson                                             */
/* Description: A symbol table module to associate string keys with  */
/*              generic values (hash table implementation)           */
/*********************************************************************/

/*********************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/* /\* DEBUG *\/ */
/* #include <stdio.h> */

/*********************************************************************/

enum NumBuckets {
   BUCKET_1 = 509,
   BUCKET_2 = 1021,
   BUCKET_3 = 2039,
   BUCKET_4 = 4093,
   BUCKET_5 = 8191,
   BUCKET_6 = 16381,
   BUCKET_7 = 32749,
   BUCKET_8 = 65521
};

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
   /* array of buckets */
   struct Binding **buckets;

   /* number of bindings stored in the SymTable */
   size_t size;

   /* number of buckets */
   enum NumBuckets bucketCount;
   
};
      

/*********************************************************************/

/*
 * Return a hash code for pcKey that is between 0 and uBucketCount - 1,
 * inclusive.
 */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
   const size_t HASH_MULTIPLIER = 65599;
   
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++) {
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
   }

   return uHash % uBucketCount;
}

/* static void printAsString(SymTable_T oSymTable) { */
/*    struct Binding *current; */
/*    int i = 0; */

/*    assert(oSymTable != NULL); */

/*    fflush(stdout); */
/*    printf( */
/*       "*************************************************************
\n" */
/*       ); */
/*    for (; i < (int)oSymTable->bucketCount; i++) { */
/*       current = oSymTable->buckets[i]; */
/*       if (current) { */
/*          printf("\n[%d]: ", i); */
/*       } */

/*       while (current != NULL) { */
/*          printf("(%s , %s), ", current->key, (char *)current->val);
 */
/*          current = current->next; */
/*       } */
/*    } */
/*    printf("\n"); */
/*    printf( */
/*       "*************************************************************
\n" */
/*       ); */
/* } */

/*
 * Puts a new binding at the end of a linked list beginning at the 
 * specified index. Takes an array of Binding pointers.
 */
static void SymTable_listPut(struct Binding **buckets,
                           int index, struct Binding *b) {
   struct Binding *current;

   assert(buckets != NULL);
   assert(b != NULL);
   
   if (buckets[index] == NULL) {
      buckets[index] = b;
      return;
   }
   
   current = buckets[index];
   while (current->next != NULL) {
      current = current->next;
   }

   current->next = b;
}

/*
 * Grows a hash table to the next size in the specified sequence 
 * if possible. If the end of the sequence is reached, does nothing.
 * Takes a symbol table oSymTable.
 */
static void SymTable_expand(SymTable_T oSymTable) {
   struct Binding **newBuckets;
   enum NumBuckets newCount;
   int i;
   struct Binding *current;
   struct Binding *previous;
   int index;
   
   assert(oSymTable != NULL);

   /* /\* DEBUG *\/ */
   /* printAsString(oSymTable); */
   
   /* find new count */
   switch(oSymTable->bucketCount) {
      case BUCKET_1:
         newCount = BUCKET_2;
         break;
      case BUCKET_2:
         newCount = BUCKET_3;
         break;
      case BUCKET_3:
         newCount = BUCKET_4;
         break;
      case BUCKET_4:
         newCount = BUCKET_5;
         break;
      case BUCKET_5:
         newCount = BUCKET_6;
         break;
      case BUCKET_6:
         newCount = BUCKET_7;
         break;
      case BUCKET_7:
         newCount = BUCKET_8;
         break;
      default:
         return;
   }

   /* allocate for buckets */
   newBuckets =
      (struct Binding**) calloc((size_t) newCount,
                                sizeof(struct Binding *));
   if (newBuckets == NULL) {
      return;
   }

   /* rehash all bindings into new buckets */
   for (i = 0; i < (int) oSymTable->bucketCount; i++) {
      current = oSymTable->buckets[i];
      while (current) {
         /* rehash */
         index = (int)(SymTable_hash(current->key, (size_t)newCount))
            % (int)newCount;
         /* erase next pointer */
         previous = current;
         current = current->next;
         
         previous->next = NULL;

         /* put in list */
         SymTable_listPut(newBuckets, index, previous);
      }
   }

   free(oSymTable->buckets);
   oSymTable->buckets = newBuckets;
   oSymTable->bucketCount = newCount;

   /* /\* DEBUG *\/ */
   /* printAsString(oSymTable); */
}

/*********************************************************************/

/*
 * Construct a new SymTable_T. Return NULL if memory is insufficient.
 */
SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;
   struct Binding **buckets;

   /* allocate for st */
   oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) {
      return NULL;
   }
   /* allocate for buckets */
   buckets =
      (struct Binding**) calloc((size_t)BUCKET_1,
                                sizeof(struct Binding *));
   if (buckets == NULL) {
      free(oSymTable);
      return NULL;
   }

   oSymTable->buckets = buckets;
   oSymTable->size = 0;
   oSymTable->bucketCount = BUCKET_1;
   
   return oSymTable;
}

/*
 * Frees all memory previously allocated for a SymTable_T
 */
void SymTable_free(SymTable_T oSymTable) {
   struct Binding *previous;
   struct Binding *current;
   int i = 0;

   assert(oSymTable != NULL);

   for (; i < (int)oSymTable->bucketCount; i++) {
      if ((previous = oSymTable->buckets[i]) == NULL) {
         continue;
      }
      current = previous->next;
      while (current != NULL) {
         free(previous->key);
         free(previous);
         previous = current;
         current = current->next;
      }
      free(previous->key);
      free(previous);
   }
   free(oSymTable->buckets);
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
   char *keyCopy;
   int keyLen;
   int bucketIdx;
   struct Binding *newBind;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   if (SymTable_contains(oSymTable, pcKey)) {
      return 0;
   }

   newBind = (struct Binding*)calloc(1, sizeof(struct Binding));
   if (newBind == NULL) {
      return 0;
   }

   /* Duplicate key */
   keyLen = (int) strlen(pcKey);
   keyCopy = (char *) malloc((size_t)(keyLen + 1));
   if (keyCopy == NULL) {
      return 0;
   }
   strcpy(keyCopy, pcKey);
   
   /* put key in */
   newBind->key = keyCopy;
   newBind->val = (void *) pvValue;

   bucketIdx = (int)(SymTable_hash(keyCopy, (size_t)oSymTable->bucketCount))
      % (int)oSymTable->bucketCount;
   SymTable_listPut(oSymTable->buckets, bucketIdx, newBind);
   oSymTable->size++;

   if ((int) oSymTable->size > (int) oSymTable->bucketCount) {
      SymTable_expand(oSymTable);
   }
   
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
   int index;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* Find and replace binding */
   index = (int)(SymTable_hash(pcKey, (size_t)oSymTable->bucketCount))
      % (int)oSymTable->bucketCount;
   
   current = oSymTable->buckets[index];
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
   int index;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   index = (int)(SymTable_hash(pcKey, (size_t)oSymTable->bucketCount))
      % (int)oSymTable->bucketCount;

   current = oSymTable->buckets[index];
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
   int index;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   index = (int)(SymTable_hash(pcKey, (size_t)oSymTable->bucketCount))
      % (int)oSymTable->bucketCount;
   
   current = oSymTable->buckets[index];
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
   int index;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   index = (int)(SymTable_hash(pcKey, (size_t)oSymTable->bucketCount))
      % (int)oSymTable->bucketCount;

   current = oSymTable->buckets[index];
   /* if not present */
   if (!current) {
      return NULL;
   }
   /* if first */
   if (strcmp(current->key, pcKey) == 0) {
      removedValue = current->val;
      oSymTable->buckets[index] = current->next;

      free(current->key);
      free(current);
      current = NULL;

      oSymTable->size--;
      return removedValue;
   }
   /* in a linked list of collided Bindings */
   previous = current;
   current = current->next;
   while(current != NULL) {
      if (strcmp(current->key, pcKey) == 0) {
         removedValue = current->val;
         previous->next = current->next;

         free(current->key);
         free(current);
         current = NULL;

         oSymTable->size--;
         return removedValue;
      }

      previous = current;
      current = current->next;
   }

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
   int i = 0;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (; i < (int)oSymTable->bucketCount; i++) {
      current = oSymTable->buckets[i];

      while (current != NULL) {
         (*pfApply)((current->key), (current->val), (void *) pvExtra);
         current = current->next;
      }
   }
}

/*********************************************************************/

#ifdef DEBUG

/*********************************************************************/

/* #include <stdio.h> */

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

   /* SPECIAL */

/*********************************************************************/
   
   return 0;
}

/*********************************************************************/

#endif

/*********************************************************************/
