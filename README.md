# HashSet implementation in C

A very performant, fairly lightweight `HashSet` implementation in C.

> **_NOTE:_**  This `HashSet` implementation solely relies on a "Array of `Linked List`s" implementation. "Treeifying" a node after a certain threshold yielded a negative performance impact from the extensive testing needed. Therefore, the current implementation does not support "treeifying".

This document provides an overview and detailed description of the functions available in this `HashSet` implementation.

## Overview

This `HashSet` implementation provides a flexible hash set with functions to create, manipulate, and destroy hash sets. It supports both default and custom hash functions.

## Typedefs
```c
typedef struct HashSet *HashSet;
typedef size_t (*hash)(const void *key, size_t key_size);
```


## Function Overview
```c
HashSet hs_create(size_t hs_capacity, size_t key_size, hash hash_func);
int hs_destroy(HashSet hs);
bool hs_contains(HashSet hs, const void *key);
int hs_add(HashSet hs, const void *key);
size_t hs_size(HashSet hs);
int hs_remove(HashSet hs, const void *key);
```
## Function Reference

```c 
HashSet hs_create(size_t hs_capacity, size_t key_size, hash hash_func)
```

**Description**: Creates a new `HashSet` with a hash function.

> **_NOTE:_** Recommended way to create a `HashSet` is to use a custom hash function to avoid unwanted collisions.

- **Parameters**:
  - `hs_capacity`: The initial capacity of the `HashSet`.
  - `key_size`: The size of the key.
  - `hash_func`: The hash function be be used, or `NULL` for generic hashing.
  
- **Returns**: A new `HashSet`.

---

```c
int hs_destroy(HashSet hs);
```

**Description**: Destroys the `HashSet`.

- **Parameters**:
  - `hs`: The `HashSet` to be destroyed.
  
- **Returns**: A success code.

---

```c
bool hs_contains(HashSet hs, const void *key);
```

**Description**: Tests if the `HashSet` contains the specified key.

- **Parameters**:
  - `hs`: The `HashSet`.
  - `key`: The key to be tested.
  
- **Returns**: True or Falsehood.

---

```c
int hs_add(HashSet hs, const void *key);
```

**Description**: Adds a new key to the `HashSet`.

- **Parameters**:
  - `hs`: The `HashSet`.
  - `key`: The key to be added.
  
- **Returns**: A sucess code.

---

```c
size_t hs_size(HashSet hs);
```

**Description**: Returns the size of the `HashSet`.

- **Parameters**:
  - `hs`: The `HashSet`.
  
- **Returns**: The size.
  
---

```c
int hs_remove(HashSet hs, const void *key);
```

**Description**: Removes the key from the HashSet.

- **Parameters**:
  - `hs`: The `HashSet`.
  - `key`: The key to be removed.
  
- **Returns**: A success code.

---

## Error Codes

- `0`: Success.
- Non-zero values indicate errors.

---

## Example

Here is a basic example demonstrating how to use the `HashSet` functions:

```c
#include "HashSet.h"

int main()
{
    // Create a HashSet
    HashSet hs = hs_create(10, sizeof(int), NULL);
    
    // Add a key-value pair
    int key = 1;
    hs_add(hs, &key);
    
    // Check if the HashSet contains the key
    printf("%s\n", hs_contains(hs, &key) ? "true" : "false");
    
    // Destroy the HashSet
    hs_destroy(hs);
    
    return 0;
}
