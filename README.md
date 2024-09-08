# HashSet implementation in C

A very performant, fairly lightweight `HashSet` implementation in C.

> [!NOTE]
> This `HashSet` implementation solely relies on a "Open Adressing" implementation. A "Array of `Linked List`s" implementation and "Treeifying" a node after a certain threshold yielded a negative performance impact from the extensive testing needed. Therefore, the current implementation uses "Open Adressing".

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

> [!TIP]
> Recommended way to create a `HashSet` is to use a custom hash function to avoid unwanted collisions.

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

## Micro Benchmarks

> [!WARNING]
> Micro benchmarks can be misleading, and performance can vary from system to system.

> [!NOTE]
> All implementation used a `struct/class` containg a randomly generated `string` as the key. Same hashing function was used in each benchmark (DJB2). Each micro benchmark is was run 100 times and the average time for each operation was taken. 

### My own HashSet

> [!NOTE]
> Used gcc -std=c2x -Ofast hashset.h hashset.c main.c for compilation.

| Operation   | Average Time (100,000 elements) |
|-------------|---------------------------------|
| Insertion   | 0.001872 seconds                |
| Lookup      | 0.001362 seconds                |
| Deletion    | 0.001498 seconds                |

### C++'s std::unordered_set

> [!NOTE]
> Used g++ -Ofast main.cpp.

| Operation   | Average Time (100,000 elements) |
|-------------|---------------------------------|
| Insertion   | 0.00908522 seconds              |
| Lookup      | 0.000315724 seconds             |
| Deletion    | 0.005854 seconds                |


### Java's HashSet

> [!NOTE]
> Used OpenJDK 22.

| Operation   | Average Time (100,000 elements) |
|-------------|---------------------------------|
| Insertion   | 0.006076 seconds                |
| Lookup      | 0.001066 seconds                |
| Deletion    | 0.001115 seconds                |

### Python's Set

> [!NOTE]
> Used CPython 3.11.9.

| Operation   | Average Time (100,000 elements) |
|-------------|---------------------------------|
| Insertion   | 0.007460 seconds                |
| Lookup      | 0.003630 seconds                |
| Deletion    | 0.003446 seconds                |

---

## Example

Here is a basic example demonstrating how to use the `HashSet` functions:

```c
#include <stdio.h>
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
