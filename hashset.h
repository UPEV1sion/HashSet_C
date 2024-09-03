//
// Created by Emanuel on 03.09.2024.
//

#ifndef HASHSET_H
#define HASHSET_H

#include <stddef.h>
#include <stdbool.h>

typedef struct HashSet *HashSet;
typedef size_t (*hash)(const void *key, size_t key_size);

/**
 * @brief Creates a new HashSet with a hash function.
 *
 * @param hs_capacity The initial capacity of the HashSet
 * @param key_size The sizeof value of the key
 * @param hash_func a custom hash function. Pass NULL for generic hashing.
 * @return HashSet
 */
HashSet hs_create(size_t hs_capacity, size_t key_size, hash hash_func);


/**
 * @brief Destroys the HashSet.
 *
 * @param hs The HashSet
 * @return Success code
 */
int hs_destroy(HashSet hs);


/**
 * @brief Tests if the HashSet contains the specified key.
 *
 * @param hs The HashSet
 * @param key The key
 * @return True or Falsehood
 */
bool hs_contains(HashSet hs, const void *key);

/**
 * @brief Adds a new key to the HashSet.
 *
 * @param hs The HashSet
 * @param key The key
 * @return Success code
 */
int hs_add(HashSet hs, const void *key);

/**
 * @brief Returns the size of the HashSet.
 *
 * @param hs The HashSet
 * @return The size
 */
size_t hs_size(HashSet hs);

/**
 * @brief Removes the key from the HashSet.
 *
 * @param hs The HashSet
 * @param key The key
 * @return Success code
 */
int hs_remove(HashSet hs, const void *key);

#endif //HASHSET_H
