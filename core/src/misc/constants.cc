/**
 * @file   constants.cc
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
 * This file defines the TileDB constants.
 */

#include <compressor.h>
#include <datatype.h>
#include <cfloat>
#include <climits>
#include <cstddef>
#include <cstdint>

#include "constants.h"

namespace tiledb {

namespace constants {

/** The array metadata file name. */
const char* array_metadata_filename = "__array_metadata.tdb";

/** The fragment metadata file name. */
const char* fragment_metadata_filename = "__fragment_metadata.tdb";

/** The default tile capacity. */
const uint64_t capacity = 1000000;

/** The size of a variable cell offset. */
const uint64_t cell_var_offset_size = sizeof(uint64_t);

/** The type of a variable cell offset. */
const Datatype cell_var_offset_type = Datatype::UINT64;

/** A special value indicating varibale size. */
const uint64_t var_size = UINT64_MAX;

/** The default compressor for the offsets of variable-sized cells. */
Compressor cell_var_offsets_compression = Compressor::DOUBLE_DELTA;

/** The default compression level for the offsets of variable-sized cells. */
int cell_var_offsets_compression_level = -1;

/** The default compressor for the coordinates. */
Compressor coords_compression = Compressor::DOUBLE_DELTA;

/** The default compressor for real coordinates. */
Compressor real_coords_compression = Compressor::BLOSC_ZSTD;

/** The default compression level for the coordinates. */
int coords_compression_level = -1;

/** The array filelock name. */
const char* array_filelock_name = "__array_lock.tdb";

/** The special value for an empty int32. */
const int empty_int32 = INT_MAX;

/** The special value for an empty int64. */
const int64_t empty_int64 = INT64_MAX;

/** The special value for an empty float32. */
const float empty_float32 = FLT_MAX;

/** The special value for an empty float64. */
const double empty_float64 = DBL_MAX;

/** The special value for an empty char. */
const char empty_char = CHAR_MAX;

/** The special value for an empty int8. */
const int8_t empty_int8 = INT8_MAX;

/** The special value for an empty uint8. */
const uint8_t empty_uint8 = UINT8_MAX;

/** The special value for an empty int16. */
const int16_t empty_int16 = INT16_MAX;

/** The special value for an empty uint16. */
const uint16_t empty_uint16 = UINT16_MAX;

/** The special value for an empty uint32. */
const uint32_t empty_uint32 = UINT32_MAX;

/** The special value for an empty uint64. */
const uint64_t empty_uint64 = UINT64_MAX;

/** The file suffix used in TileDB. */
const char* file_suffix = ".tdb";

/** Default datatype for a generic tile. */
const Datatype generic_tile_datatype = Datatype::CHAR;

/** Default compressor for a generic tile. */
Compressor generic_tile_compressor = Compressor::BLOSC_ZSTD;

/** Default compression level for a generic tile. */
int generic_tile_compression_level = -1;

/** Default cell size for a generic tile. */
uint64_t generic_tile_cell_size = sizeof(char);

/** The group file name. */
const char* group_filename = "__tiledb_group.tdb";

/** The initial internal buffer size for the case of sparse arrays. */
const uint64_t internal_buffer_size = 10000000;

/** The buffer size for each attribute used in consolidation. */
const uint64_t consolidation_buffer_size = 10000000;

/** The maximum number of bytes written in a single I/O. */
const uint64_t max_write_bytes = INT_MAX;

/** The maximum name length. */
const unsigned name_max_len = 256;

/** The size of the buffer that holds the sorted cells. */
const uint64_t sorted_buffer_size = 10000000;

/** The size of the buffer that holds the sorted variable cells. */
const uint64_t sorted_buffer_var_size = 10000000;

/** Special value indicating a variable number of elements. */
const unsigned int var_num = UINT_MAX;

/** String describing no compression. */
const char* no_compression_str = "NO_COMPRESSION";

/** String describing GZIP. */
const char* gzip_str = "GZIP";

/** String describing ZSTD. */
const char* zstd_str = "ZSTD";

/** String describing LZ4. */
const char* lz4_str = "LZ4";

/** String describing BLOSC. */
const char* blosc_str = "BLOSC_LZ";

/** String describing BLOSC_LZ4. */
const char* blosc_lz4_str = "BLOSC_LZ4";

/** String describing BLOSC_LZ4HC. */
const char* blosc_lz4hc_str = "BLOSC_LZ4HC";

/** String describing BLOSC_SNAPPY. */
const char* blosc_snappy_str = "BLOSC_SNAPPY";

/** String describing BLOSC_ZLIB. */
const char* blosc_zlib_str = "BLOSC_ZLIB";

/** String describing BLOSC_ZSTD. */
const char* blosc_zstd_str = "BLOSC_ZSTD";

/** String describing RLE. */
const char* rle_str = "RLE";

/** String describing BZIP2. */
const char* bzip2_str = "BZIP2";

/** String describing DOUBLE_DELTA. */
const char* double_delta_str = "DOUBLE_DELTA";

/** The string representation for type int32. */
const char* int32_str = "INT32";

/** The string representation for type int64. */
const char* int64_str = "INT64";

/** The string representation for type float32. */
const char* float32_str = "FLOAT32";

/** The string representation for type float64. */
const char* float64_str = "FLOAT64";

/** The string representation for type char. */
const char* char_str = "CHAR";

/** The string representation for type int8. */
const char* int8_str = "INT8";

/** The string representation for type uint8. */
const char* uint8_str = "UINT8";

/** The string representation for type int16. */
const char* int16_str = "INT16";

/** The string representation for type uint16. */
const char* uint16_str = "UINT16";

/** The string representation for type uint32. */
const char* uint32_str = "UINT32";

/** The string representation for type uint64. */
const char* uint64_str = "UINT64";

/** The string representation for the dense array type. */
const char* dense_str = "dense";

/** The string representation for the sparse array type. */
const char* sparse_str = "sparse";

/** The string representation for the column-major layout. */
const char* col_major_str = "col-major";

/** The string representation for the row-major layout. */
const char* row_major_str = "row-major";

/** The string representation for the global order layout. */
const char* global_order_str = "global-order";

/** The string representation for the unordered layout. */
const char* unordered_str = "unordered";

/** The string representation of null. */
const char* null_str = "null";

/** The version in format { major, minor, revision }. */
const int version[3] = {1, 2, 0};

/** The size of a tile chunk. */
const uint64_t tile_chunk_size = INT_MAX;

/* ********************************* */
/*           RESERVED NAMES          */
/* ********************************* */

/** Special name reserved for the coordinates attribute. */
const char* coords = "__coords";

/**
 * The name of the first key dimension (recall that a key in a
 * key-value store is hashed into a 16-byte MD5 digest, which
 * is represented as a 2-dimensional uint64_t value.
 */
const char* key_dim_1 = "__key_dim_1";

/**
 * The name of the second key dimension (recall that a key in a
 * key-value store is hashed into a 16-byte MD5 digest, which
 * is represented as a 2-dimensional uint64_t value.
 */
const char* key_dim_2 = "__key_dim_2";

/** Reserved name for the key attribute in a key-value store. */
const char* key_attr_name = "__key";

/** Reserved name for the key type attribute in a key-value store. */
const char* key_type_attr_name = "__key_type";

/** Name of special empty file that indicates a key-value store. */
const char* kv_filename = "__kv.tdb";

/* ********************************* */
/*             FUNCTIONS             */
/* ********************************* */

bool reserved_name(const std::string& name) {
  return (
      name == constants::key_attr_name ||
      name == constants::key_type_attr_name || name == constants::coords ||
      name == constants::key_dim_1 || name == constants::key_dim_2);
}

}  // namespace constants

}  // namespace tiledb
