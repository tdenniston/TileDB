/**
 * @file   kv_query.h
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
 * This file defines class KVQuery.
 */

#ifndef TILEDB_KV_QUERY_H
#define TILEDB_KV_QUERY_H

#include "query.h"

#include <vector>

namespace tiledb {

/**
 * Stores information about a key-value query. This is just a wrapper of an
 * array query.
 */
class KVQuery {
 public:
  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */

  /** Constructor. */
  KVQuery();

  /** Destructor. */
  ~KVQuery();

  /* ********************************* */
  /*                 API               */
  /* ********************************* */

  /**
   * Initializes the key-value query.
   *
   * @param storage_manager The storage manager.
   * @param array_metadata The array metadata.
   * @param fragment_metadata The metadata of the involved fragments.
   * @param type The query type.
   * @param keys The keys involved in the query.
   * @param attributes The attributes the query will be constrained on.
   * @param attribute_num The number of attributes.
   * @param buffers The query buffers with a one-to-one correspondences with
   *     the specified attributes. In a read query, the buffers will be
   *     populated with the query results. In a write query, the buffer
   *     contents will be appropriately written in a new fragment.
   * @param buffer_sizes The corresponding buffer sizes.
   * @return Status
   */
  Status init(
      StorageManager* storage_manager,
      const ArrayMetadata* array_metadata,
      const std::vector<FragmentMetadata*>& fragment_metadata,
      QueryType type,
      Keys* keys,
      const char** attributes,
      unsigned int attribute_num,
      void** buffers,
      uint64_t* buffer_sizes);

  /** Returns the underlying array query. */
  Query* query() const;

  /**
   * Resets the sizes in the user buffers. This is important (mainly for
   * reads), since the underlying array query never alters the user buffer
   * sizes.
   */
  void reset_user_buffer_sizes();

 private:
  /* ********************************* */
  /*         PRIVATE ATTRIBUTES        */
  /* ********************************* */

  /** The buffers that are input to the array query. */
  std::vector<void*> buffers_;

  /** The corresponding sizes to `buffers_`.  */
  std::vector<uint64_t> buffer_sizes_;

  /**
   * The keys participating in the key-value query. Note that the KVQuery object
   * does not own this member, so it does not need to deallocate it.
   */
  Keys* keys_;

  /** The underlying array query. */
  Query* query_;

  /** The type of the query. */
  QueryType type_;

  /** The buffer sizes provided by the user for the query. */
  uint64_t* user_buffer_sizes_;

  /** Number of user buffer sizes provided. */
  unsigned int user_buffer_sizes_num_;

  /* ********************************* */
  /*           PRIVATE METHODS         */
  /* ********************************* */

  /**
   * Computes the coordinates as MD5 digests of the keys.
   *
   * @return Status
   */
  Status compute_coords() const;

  /**
   * (Applicable only to read queries)
   * Computes a subarray query based on the MD5 digest of the input key.
   *
   * @param subarray The subarray to be created.
   * @return Status
   */
  Status compute_subarray(void** subarray) const;

  /**
   * Retrieves the ids of the attributes that will be involved in the
   * underlying array query.
   *
   * @param array_metadata The array metadata.
   * @param attributes The attribute names inserted by the user.
   * @param attribute_num The number of attributes.
   * @param attribute_ids The attribute ids to be retrieved.
   * @return Status
   */
  Status get_attribute_ids(
      const ArrayMetadata* array_metadata,
      const char** attributes,
      unsigned int attribute_num,
      std::vector<unsigned int>* attribute_ids) const;

  /**
   * Sets the internal `buffers_` and `buffer_sizes`. Note that this is
   * necessary because a key-value query creates different buffers to
   * give as input to the underlying array query than the ones provided
   * by the users.
   *
   * @param array_metadata The underlying array metadata.
   * @param attribute_ids The ids of the attributes involved in the query.
   * @param buffers The user buffers provided for the key-value query.
   * @param buffer_sizes The corresponding sizes of `buffers_`.
   * @return Status
   */
  Status set_buffers(
      const ArrayMetadata* array_metadata,
      const std::vector<unsigned int>& attribute_ids,
      void** buffers,
      uint64_t* buffer_sizes);
};

}  // namespace tiledb

#endif  // TILEDB_KV_QUERY_H
