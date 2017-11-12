/**
 * @file   keys.h
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 TileDB, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * This file defines class Keys.
 */

#ifndef TILEDB_KEYS_H
#define TILEDB_KEYS_H

#include "buffer.h"
#include "datatype.h"
#include "status.h"

namespace tiledb {

/**
 * Manages a set of keys to be used in a key-value store. A key is
 * characterized by a value, a type, and its size in bytes.
 */
class Keys {
 public:
  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */

  /** Constructor. */
  Keys();

  /** Destructor. */
  ~Keys();

  /* ********************************* */
  /*                API                */
  /* ********************************* */

  /**
   * Adds a key to the structure.
   *
   * @param key The key value.
   * @param type The key type.
   * @param size The key size.
   * @return Status
   */
  Status add_key(void* key, Datatype type, uint64_t size);

  /** Returns the number of keys currently stored in the object. */
  uint64_t key_num() const;

  /** Returns the data of the `keys_` buffer. */
  void* keys_data() const;

  /** Returns the data of the `keys_var_` buffer. */
  void* keys_var_data() const;

  /** Returns the size (in bytes) of the `keys_` buffer. */
  uint64_t keys_size() const;

  /** Returns the size (in bytes) of the `keys_var_` buffer. */
  uint64_t keys_var_size() const;

  /** Returns the data of the `types_` buffer. */
  void* types_data() const;

  /** Returns the size (in bytes) of the `types_` buffer. */
  uint64_t types_size() const;

 private:
  /* ********************************* */
  /*         PRIVATE ATTRIBUTES        */
  /* ********************************* */

  /** Number of keys stored in the object. */
  uint64_t key_num_;

  /** Stores the key value offsets in binary format. */
  Buffer keys_;

  /** Stores the key (variable-sized) values in binary format. */
  Buffer keys_var_;

  /** Stores the key types in binary format. */
  Buffer types_;
};

}  // namespace tiledb

#endif  // TILEDB_KEYS_H
