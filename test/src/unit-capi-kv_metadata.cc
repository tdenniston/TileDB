/**
 * @file unit-capi-kv_metadata.cc
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
 * Tests for the C API tiledb_kv_metadata_t spec.
 */

#include <constants.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

#include "catch.hpp"
#include "posix_filesystem.h"
#include "tiledb.h"
#include "uri.h"

struct KVMetadataFx {
// Constant parameters
#ifdef HAVE_HDFS
  const std::string URI_PREFIX = "hdfs://";
  const std::string TEMP_DIR = "/tiledb_test/";
#else
  const std::string URI_PREFIX = "file://";
  const std::string TEMP_DIR = tiledb::posix::current_dir() + "/";
#endif
  const std::string GROUP = "test_group/";
  const std::string KV_NAME = "kv";
  const std::string KV_PATH = URI_PREFIX + TEMP_DIR + GROUP + KV_NAME;
  const std::string KV_PATH_REAL = tiledb::URI(KV_PATH).to_string();
  const char* ATTR_NAME = "a";
  const tiledb_datatype_t ATTR_TYPE = TILEDB_INT32;
  const char* ATTR_TYPE_STR = "INT32";
  const tiledb_compressor_t ATTR_COMPRESSOR = TILEDB_NO_COMPRESSION;
  const char* ATTR_COMPRESSOR_STR = "NO_COMPRESSION";
  const int ATTR_COMPRESSION_LEVEL = -1;
  const char* ATTR_COMPRESSION_LEVEL_STR = "-1";
  const unsigned int CELL_VAL_NUM = 1;
  const char* CELL_VAL_NUM_STR = "1";

  // Key-value metadata object under test
  tiledb_kv_metadata_t* kv_metadata_;

  // TileDB context
  tiledb_ctx_t* ctx_;

  KVMetadataFx() {
    // Error code
    int rc;

    // Array metadata not set yet
    kv_metadata_ = nullptr;

    // Initialize context
    rc = tiledb_ctx_create(&ctx_);
    assert(rc == TILEDB_OK);

    // Create group, delete it if it already exists
    if (dir_exists(TEMP_DIR + GROUP)) {
      bool success = remove_dir(TEMP_DIR + GROUP);
      assert(success);
    }
    rc = tiledb_group_create(ctx_, (URI_PREFIX + TEMP_DIR + GROUP).c_str());
    assert(rc == TILEDB_OK);
  }

  ~KVMetadataFx() {
    // Free array_metadata metadata
    if (kv_metadata_ != nullptr)
      tiledb_kv_metadata_free(ctx_, kv_metadata_);

    // Free TileDB context
    tiledb_ctx_free(ctx_);

    // Remove the temporary group
    bool success = remove_dir(TEMP_DIR + GROUP);
    assert(success == true);
  }

  bool dir_exists(std::string path) {
#ifdef HAVE_HDFS
    std::string cmd = std::string("hadoop fs -test -d ") + path;
#else
    std::string cmd = std::string("test -d ") + path;
#endif
    return (system(cmd.c_str()) == 0);
  }

  bool remove_dir(std::string path) {
#ifdef HAVE_HDFS
    std::string cmd = std::string("hadoop fs -rm -r -f ") + path;
#else
    std::string cmd = std::string("rm -r -f ") + path;
#endif
    return (system(cmd.c_str()) == 0);
  }

  void create_kv() {
    int rc;

    // Create key-value metadata with invalid URI
    rc = tiledb_kv_metadata_create(ctx_, &kv_metadata_, "file://my_kv");
    REQUIRE(rc != TILEDB_OK);

    // Create key-value metadata
    rc = tiledb_kv_metadata_create(ctx_, &kv_metadata_, KV_PATH.c_str());
    REQUIRE(rc == TILEDB_OK);

    // Set attribute
    tiledb_attribute_t* attr;
    rc = tiledb_attribute_create(ctx_, &attr, ATTR_NAME, ATTR_TYPE);
    REQUIRE(rc == TILEDB_OK);
    rc = tiledb_kv_metadata_add_attribute(ctx_, kv_metadata_, attr);
    REQUIRE(rc == TILEDB_OK);

    // Add attribute with reserved name
    tiledb_attribute_t* attr_r;
    rc = tiledb_attribute_create(
        ctx_, &attr_r, tiledb::constants::key_attr_name, ATTR_TYPE);
    REQUIRE(rc == TILEDB_OK);
    rc = tiledb_kv_metadata_add_attribute(ctx_, kv_metadata_, attr_r);
    REQUIRE(rc != TILEDB_OK);

    // Clean up
    tiledb_attribute_free(ctx_, attr);
    tiledb_attribute_free(ctx_, attr_r);

    // Create the array
    rc = tiledb_kv_create(ctx_, kv_metadata_);
    REQUIRE(rc == TILEDB_OK);
  }
};

TEST_CASE_METHOD(
    KVMetadataFx,
    "C API: Test key-value metadata creation and retrieval",
    "[capi] [key-value metadata]") {
  create_kv();

  // Load key-value metadata from the disk
  tiledb_kv_metadata_t* kv_metadata;
  int rc = tiledb_kv_metadata_load(ctx_, &kv_metadata, KV_PATH.c_str());
  REQUIRE(rc == TILEDB_OK);

  // Check name
  const char* name;
  rc = tiledb_kv_metadata_get_name(ctx_, kv_metadata, &name);
  REQUIRE(rc == TILEDB_OK);
  CHECK_THAT(name, Catch::Equals(KV_PATH_REAL));

  // Check attribute
  int attr_it_done;
  tiledb_attribute_iter_t* attr_it;
  rc = tiledb_kv_attribute_iter_create(ctx_, kv_metadata, &attr_it);
  REQUIRE(rc == TILEDB_OK);

  rc = tiledb_attribute_iter_done(ctx_, attr_it, &attr_it_done);
  REQUIRE(rc == TILEDB_OK);
  CHECK(!attr_it_done);

  const tiledb_attribute_t* attr;
  rc = tiledb_attribute_iter_here(ctx_, attr_it, &attr);
  REQUIRE(rc == TILEDB_OK);

  const char* attr_name;
  rc = tiledb_attribute_get_name(ctx_, attr, &attr_name);
  REQUIRE(rc == TILEDB_OK);
  CHECK_THAT(attr_name, Catch::Equals(ATTR_NAME));

  tiledb_datatype_t attr_type;
  rc = tiledb_attribute_get_type(ctx_, attr, &attr_type);
  REQUIRE(rc == TILEDB_OK);
  CHECK(attr_type == ATTR_TYPE);

  tiledb_compressor_t attr_compressor;
  int attr_compression_level;
  rc = tiledb_attribute_get_compressor(
      ctx_, attr, &attr_compressor, &attr_compression_level);
  REQUIRE(rc == TILEDB_OK);
  CHECK(attr_compressor == ATTR_COMPRESSOR);
  CHECK(attr_compression_level == ATTR_COMPRESSION_LEVEL);

  unsigned int cell_val_num;
  rc = tiledb_attribute_get_cell_val_num(ctx_, attr, &cell_val_num);
  REQUIRE(rc == TILEDB_OK);
  CHECK(cell_val_num == CELL_VAL_NUM);

  rc = tiledb_attribute_iter_next(ctx_, attr_it);
  REQUIRE(rc == TILEDB_OK);
  rc = tiledb_attribute_iter_done(ctx_, attr_it, &attr_it_done);
  REQUIRE(rc == TILEDB_OK);
  CHECK(attr_it_done);

  rc = tiledb_attribute_iter_first(ctx_, attr_it);
  REQUIRE(rc == TILEDB_OK);
  rc = tiledb_attribute_iter_here(ctx_, attr_it, &attr);
  REQUIRE(rc == TILEDB_OK);
  rc = tiledb_attribute_get_name(ctx_, attr, &attr_name);
  REQUIRE(rc == TILEDB_OK);
  CHECK_THAT(attr_name, Catch::Equals(ATTR_NAME));

  // Check dump as key-value
  std::stringstream dump_kv_ss;
  dump_kv_ss << "- Key-value store name: " << tiledb::URI(KV_PATH).to_string()
             << "\n\n"
             << "### Attribute ###\n"
             << "- Name: " << ATTR_NAME << "\n"
             << "- Type: " << ATTR_TYPE_STR << "\n"
             << "- Compressor: " << ATTR_COMPRESSOR_STR << "\n"
             << "- Compression level: " << ATTR_COMPRESSION_LEVEL_STR << "\n"
             << "- Cell val num: " << CELL_VAL_NUM_STR << "\n";
  std::string dump_kv_str = dump_kv_ss.str();
  FILE* gold_kv_fout = fopen("gold_kv_fout.txt", "w");
  const char* dump_kv = dump_kv_str.c_str();
  fwrite(dump_kv, sizeof(char), strlen(dump_kv), gold_kv_fout);
  fclose(gold_kv_fout);
  FILE* fout_kv = fopen("kv_fout.txt", "w");
  tiledb_kv_metadata_dump(ctx_, kv_metadata, fout_kv);
  fclose(fout_kv);
  CHECK(!system("diff gold_kv_fout.txt kv_fout.txt"));
  system("rm gold_kv_fout.txt kv_fout.txt");

  // Check dump the underlying sparse array
  std::stringstream dump_ss;
  dump_ss << "- Array name: " << tiledb::URI(KV_PATH).to_string() << "\n"
          << "- Array type: sparse\n"
          << "- Key-value: true\n"
          << "- Cell order: row-major\n"
          << "- Tile order: row-major\n"
          << "- Capacity: " << tiledb::constants::capacity << "\n"
          << "- Coordinates compressor: DOUBLE_DELTA\n"
          << "- Coordinates compression level: -1\n\n"
          << "=== Domain ===\n"
          << "- Dimensions type: UINT64\n\n"
          << "### Dimension ###\n"
          << "- Name: " << tiledb::constants::key_dim_1 << "\n"
          << "- Domain: [0," << UINT64_MAX << "]\n"
          << "- Tile extent: null\n\n"
          << "### Dimension ###\n"
          << "- Name: " << tiledb::constants::key_dim_2 << "\n"
          << "- Domain: [0," << UINT64_MAX << "]\n"
          << "- Tile extent: null\n\n"
          << "### Attribute ###\n"
          << "- Name: " << ATTR_NAME << "\n"
          << "- Type: " << ATTR_TYPE_STR << "\n"
          << "- Compressor: " << ATTR_COMPRESSOR_STR << "\n"
          << "- Compression level: " << ATTR_COMPRESSION_LEVEL_STR << "\n"
          << "- Cell val num: " << CELL_VAL_NUM_STR << "\n\n"
          << "### Attribute ###\n"
          << "- Name: " << tiledb::constants::key_attr_name << "\n"
          << "- Type: CHAR\n"
          << "- Compressor: BLOSC_ZSTD\n"
          << "- Compression level: -1\n"
          << "- Cell val num: var\n\n"
          << "### Attribute ###\n"
          << "- Name: " << tiledb::constants::key_type_attr_name << "\n"
          << "- Type: CHAR\n"
          << "- Compressor: DOUBLE_DELTA\n"
          << "- Compression level: -1\n"
          << "- Cell val num: 1\n";
  std::string dump_str = dump_ss.str();
  FILE* gold_fout = fopen("gold_fout.txt", "w");
  const char* dump = dump_str.c_str();
  fwrite(dump, sizeof(char), strlen(dump), gold_fout);
  fclose(gold_fout);
  FILE* fout = fopen("fout.txt", "w");
  tiledb_kv_metadata_dump_as_array(ctx_, kv_metadata, fout);
  fclose(fout);
  CHECK(!system("diff gold_fout.txt fout.txt"));
  system("rm gold_fout.txt fout.txt");

  // Clean up
  rc = tiledb_attribute_iter_free(ctx_, attr_it);
  REQUIRE(rc == TILEDB_OK);
  rc = tiledb_kv_metadata_free(ctx_, kv_metadata);
  REQUIRE(rc == TILEDB_OK);
}
