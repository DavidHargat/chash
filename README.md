# chash

A C implementation of a hash table.

## Interface

`htable`

| functions  | parameters                                          |
| ---------- | --------------------------------------------------- |
|`hash_get`  | `table, key_src, key_length`                        |
|`hash_set`  | `table, key_src, key_length, data_src, data_length` |
|`hash_gets` | `table, key, data`                                  |
|`hash_sets` | `table, key`                                        |

| parameter     | type       |
| ------------- | ---------- |
| `table`       | `htable *` |
| `key`         | `char *`   |
| `key_src`     | `char *`   |
| `key_length`  | `size_t`   |
| `data`        | `char *`   |
| `data_src`    | `char *`   |
| `data_length` | `size_t`   |

## Usage

Include `hash.h` in your project

`#include "hash.h"`

Create a new table.

`htable *table = create_table(32)`

Store strings in a key/value pair.

`hash_sets(table, "key", "value");`

Retrieve a string key.

`hash_gets(table, "key") // returns "value"`

Free a table (also `free()`s all data/nodes copied into it).

`free_table(table)`

