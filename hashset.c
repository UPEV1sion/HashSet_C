#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hashset.h"

//
// Created by Emanuel on 03.09.2024.
//

#define MIN_CAPACITY (1 << 4)
#define MAX_CAPACITY (1 << 30)
#define LOAD_FACTOR 0.75

typedef enum { ACTIVE, TOMBSTONE } BucketStatus;

typedef struct Bucket
{
    void *key;
    size_t hash;
    struct Bucket *next;
    BucketStatus status;
} Bucket;

struct HashSet
{
    size_t size;
    size_t capacity;
    size_t key_size;

    hash hash_func;
    Bucket **buckets;
};

Bucket* create_bucket(const void *key, const size_t key_size, const size_t hash)
{
    Bucket *bucket = malloc(sizeof(Bucket));
    assert(bucket != NULL);

    bucket->key = malloc(key_size);
    assert(bucket->key != NULL);
    memcpy(bucket->key, key, key_size);

    bucket->hash   = hash;
    bucket->status = ACTIVE;
    bucket->next   = NULL;

    return bucket;
}

size_t _hash(const void *key, const size_t key_size)
{
    //DJB2 hash func variation
    const unsigned char *data = key;
    size_t hash               = 5381;

    for (size_t i = 0; i < key_size; ++i)
    {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

double calc_load_fac(const HashSet hs)
{
    return (double) hs->size / hs->capacity;
}


int hs_resize(const HashSet hs)
{
    size_t new_capacity = hs->capacity * 2;
    if (new_capacity > MAX_CAPACITY)
        new_capacity = MAX_CAPACITY;
    if (hs->size >= MAX_CAPACITY * LOAD_FACTOR)
        return 1;
    Bucket **new_buckets = realloc(hs->buckets, new_capacity * sizeof(Bucket *));
    assert(new_buckets != NULL);
    memset(new_buckets + hs->capacity, 0, (new_capacity - hs->capacity) * sizeof(Bucket *));
    hs->buckets = new_buckets;

    for (size_t i = 0; i < hs->capacity; i++)
    {
        Bucket *bucket = new_buckets[i];
        new_buckets[i] = NULL;
        while (bucket != NULL)
        {
            Bucket *next = bucket->next;
            if (bucket->status == ACTIVE)
            {
                const size_t hash = hs->hash_func(bucket->key, hs->key_size) % new_capacity;
                bucket->next      = new_buckets[hash];
                new_buckets[hash] = bucket;
            }
            else
            {
                free(bucket->key);
                free(bucket);
            }
            bucket = next;
        }
    }
    hs->capacity = new_capacity;

    return 0;
}

HashSet hs_create(const size_t hs_capacity, const size_t key_size, const hash hash_func)
{
    const HashSet hs = malloc(sizeof(*hs));
    assert(hs != NULL);
    hs->hash_func = hash_func == NULL ? _hash : hash_func;
    hs->capacity = (hs_capacity < MIN_CAPACITY) ? MIN_CAPACITY : hs_capacity;
    hs->buckets  = calloc(hs->capacity, sizeof(Bucket *));
    assert(hs->buckets != NULL);
    hs->size       = 0;
    hs->key_size   = key_size;

    return hs;
}

int hs_destroy(const HashSet hs)
{
    for (size_t i = 0; i < hs->capacity; ++i)
    {
        for (Bucket *bucket = hs->buckets[i]; bucket != NULL;)
        {
            Bucket *next = bucket->next;
            free(bucket->key);
            free(bucket);
            bucket = next;
        }
    }
    free(hs->buckets);
    free(hs);

    return 0;
}

bool hs_contains(const HashSet hs, const void *key)
{
    const size_t hash = hs->hash_func(key, hs->key_size) % hs->capacity;
    for (Bucket *bucket = hs->buckets[hash]; bucket != NULL; bucket = bucket->next)
    {
        if (bucket->status == ACTIVE && memcmp(bucket->key, key, hs->key_size) == 0)
        {
            return true;
        }
    }

    return false;
}

int hs_add(const HashSet hs, const void *key)
{
    if (calc_load_fac(hs) > LOAD_FACTOR)
        if (hs_resize(hs) != 0)
            return 1;

    const size_t hash = hs->hash_func(key, hs->key_size) % hs->capacity;
    for (Bucket *bucket = hs->buckets[hash]; bucket != NULL; bucket = bucket->next)
    {
        if (memcmp(bucket->key, key, hs->key_size) == 0)
        {
            bucket->status = ACTIVE;
            return 0;
        }
    }

    Bucket *bucket    = create_bucket(key, hs->key_size, hash);
    bucket->next      = hs->buckets[hash];
    hs->buckets[hash] = bucket;
    hs->size++;

    return 0;
}

size_t hs_size(const HashSet hs)
{
    return hs->size;
}

int hs_remove(const HashSet hs, const void *key)
{
    const size_t hash = hs->hash_func(key, hs->key_size) % hs->capacity;
    Bucket *prev      = NULL;
    for (Bucket *bucket = hs->buckets[hash]; bucket != NULL; bucket = bucket->next)
    {
        if (memcmp(bucket->key, key, hs->key_size) == 0)
        {
            if (prev == NULL)
            {
                hs->buckets[hash] = bucket->next;
            }
            else
            {
                prev->next = bucket->next;
            }
            bucket->status = TOMBSTONE;
            hs->size--;

            return 0;
        }
        prev = bucket;
    }

    return 1;
}
