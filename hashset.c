#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hashset.h"

//
// Created by Emanuel on 05.09.2024.
//

#define MIN_CAPACITY (1 << 4)
#define MAX_CAPACITY (1 << 30)
#define LOAD_FACTOR 0.75

typedef enum { TOMBSTONE, ACTIVE } BucketStatus;

typedef struct Bucket
{
    size_t hash;
    BucketStatus status;
    char key[];
} Bucket;

struct HashSet
{
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t bucket_size;

    hash hash_func;
    Bucket *buckets;
};

static size_t default_hash(const void *key, const size_t key_size)
{
    const unsigned char *data = key;
    size_t hash               = 5381;

    for (size_t i = 0; i < key_size; ++i)
    {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

static inline double calc_load_fac(const HashSet hs)
{
    return (double) hs->size / hs->capacity;
}

static inline Bucket* get_bucket(const HashSet hs, const size_t hash)
{
    return (Bucket *) ((char *) hs->buckets + hash * hs->bucket_size);
}

static int hs_resize(const HashSet hs)
{
    size_t new_capacity = hs->capacity * 2;
    if (new_capacity > MAX_CAPACITY) new_capacity = MAX_CAPACITY;
    if (new_capacity >= MAX_CAPACITY * LOAD_FACTOR)
        return 1;

    Bucket *new_buckets = calloc(new_capacity, hs->bucket_size);
    assert(new_buckets != NULL);

    for (size_t i = 0; i < new_capacity; ++i)
    {
        Bucket *bucket = (Bucket *) ((char *) new_buckets + i * hs->bucket_size);
        bucket->status = TOMBSTONE;
        bucket->hash   = 0;
    }

    for (size_t i = 0; i < hs->capacity; i++)
    {
        Bucket *bucket = get_bucket(hs, i);
        if (bucket->status == ACTIVE)
        {
            size_t new_hash    = hs->hash_func(bucket->key, hs->key_size) % new_capacity;
            Bucket *new_bucket = (Bucket *) ((char *) new_buckets + new_hash * hs->bucket_size);
            while (new_bucket->status == ACTIVE)
            {
                new_hash   = (new_hash + 1) % new_capacity;
                new_bucket = (Bucket *) ((char *) new_buckets + new_hash * hs->bucket_size);
            }
            memcpy(new_bucket, bucket, hs->bucket_size);
        }
    }

    free(hs->buckets);
    hs->buckets  = new_buckets;
    hs->capacity = new_capacity;

    return 0;
}

HashSet hs_create(const size_t hs_capacity, const size_t key_size, const hash hash_func)
{
    const HashSet hs = malloc(sizeof(*hs));
    assert(hs != NULL);

    hs->hash_func   = hash_func == NULL ? default_hash : hash_func;
    hs->capacity    = (hs_capacity < MIN_CAPACITY) ? MIN_CAPACITY : hs_capacity;
    hs->bucket_size = sizeof(Bucket) + key_size;
    hs->buckets     = malloc(hs->capacity * hs->bucket_size);
    assert(hs->buckets != NULL);

    hs->size     = 0;
    hs->key_size = key_size;

    for (size_t i = 0; i < hs->capacity; ++i)
    {
        Bucket *bucket = get_bucket(hs, i);
        bucket->status = TOMBSTONE;
        bucket->hash   = 0;
    }

    return hs;
}

int hs_destroy(const HashSet hs)
{
    free(hs->buckets);
    free(hs);

    return 0;
}

bool hs_contains(const HashSet hs, const void *key)
{
    size_t hash             = hs->hash_func(key, hs->key_size) % hs->capacity;
    const size_t start_hash = hash;
    Bucket *bucket          = get_bucket(hs, hash);

    while (bucket->status != TOMBSTONE)
    {
        if (memcmp(bucket->key, key, hs->key_size) == 0)
        {
            return true;
        }
        hash   = (hash + 1) % hs->capacity;
        bucket = get_bucket(hs, hash);
        if (bucket == get_bucket(hs, start_hash)) break;
    }

    return false;
}

int hs_add(const HashSet hs, const void *key)
{
    if (calc_load_fac(hs) > LOAD_FACTOR)
        if (hs_resize(hs) != 0)
            return 1;

    size_t hash             = hs->hash_func(key, hs->key_size) % hs->capacity;
    const size_t start_hash = hash;
    Bucket *bucket          = get_bucket(hs, hash);

    while (bucket->status != TOMBSTONE && memcmp(bucket->key, key, hs->key_size) != 0)
    {
        hash   = (hash + 1) % hs->capacity;
        bucket = get_bucket(hs, hash);
        if (bucket == get_bucket(hs, start_hash))
        {
            return 1;
        }
    }

    if (bucket->status == ACTIVE && memcmp(bucket->key, key, hs->key_size) == 0)
    {
        memcpy(bucket->key, key, hs->key_size);

        return 0;
    }

    bucket->status = ACTIVE;
    bucket->hash   = hash;
    memcpy(bucket->key, key, hs->key_size);
    hs->size++;

    return 0;
}

size_t hs_size(const HashSet hs)
{
    return hs->size;
}

int hs_remove(const HashSet hs, const void *key)
{
    const size_t init_hash = hs->hash_func(key, hs->key_size) % hs->capacity;
    size_t current_hash    = init_hash;
    Bucket *bucket         = get_bucket(hs, current_hash);

    while (bucket->status != TOMBSTONE)
    {
        if (memcmp(bucket->key, key, hs->key_size) == 0)
        {
            bucket->status = TOMBSTONE;
            hs->size--;

            size_t next_hash    = (current_hash + 1) % hs->capacity;
            Bucket *next_bucket = get_bucket(hs, next_hash);

            while (next_bucket->status != TOMBSTONE)
            {
                const size_t next_original_hash = hs->hash_func(next_bucket->key, hs->key_size) % hs->capacity;

                if ((next_original_hash <= current_hash && current_hash < next_hash) ||
                    next_hash < next_original_hash && next_original_hash <= current_hash)
                {
                    memcpy(bucket, next_bucket, hs->bucket_size);

                    next_bucket->status = TOMBSTONE;

                    current_hash = next_hash;
                    bucket       = get_bucket(hs, current_hash);
                }
                next_hash   = (next_hash + 1) % hs->capacity;
                next_bucket = get_bucket(hs, next_hash);
            }

            return 0;
        }

        current_hash = (current_hash + 1) % hs->capacity;
        bucket       = get_bucket(hs, current_hash);

        if (current_hash == init_hash) break;
    }

    return 1;
}
