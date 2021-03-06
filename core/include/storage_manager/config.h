/**
 * @file   config.h
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
 * This file defines class Config.
 */

#ifndef TILEDB_CONFIG_H
#define TILEDB_CONFIG_H

#include "status.h"

#include <unordered_map>

namespace tiledb {

/** This class manages the TileDB configuration options. */
class Config {
 public:
  /* ********************************* */
  /*         TYPE DEFINITIONS          */
  /* ********************************* */

  /** Storage manager parameters. */
  struct SMParams {
    uint64_t array_schema_cache_size_;
    uint64_t fragment_metadata_cache_size_;
    uint64_t tile_cache_size_;
  };

  struct S3Params {
    std::string region_;
    std::string scheme_;
    std::string endpoint_override_;
    bool use_virtual_addressing_;
    uint64_t file_buffer_size_;
    long connect_timeout_ms_;
    long request_timeout_ms_;
  };

  struct HDFSParams {};

  struct VFSParams {
    S3Params s3_params_;
    HDFSParams hdfs_params_;
  };

  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */

  /** Constructor. */
  Config();

  /** Destructor. */
  ~Config();

  /* ********************************* */
  /*                API                */
  /* ********************************* */

  /** Returns the storage manager parameters. */
  SMParams sm_params() const;

  /** Returns the VFS parameters. */
  VFSParams vfs_params() const;

  /** Returns the S3 parameters. */
  S3Params s3_params() const;

  /**
   * Initializes the config. This function will return error if there is
   * any problem with the set parameters.
   */
  Status init();

  /**
   * Sets a config parameter. Note that this does not have any effect
   * until the object is initialized with `init`.
   */
  Status set(const std::string& param, const std::string& value);

  /**
   * Sets the name of the file from which the config parameters will be read
   * upon initialization.
   *
   * @note If the user sets parameter-value pairs from both a file and
   *     through the `set` function, in the case of conflicts, the parameters
   *     set through the `set` function take priority over those set via
   *     the file.
   */
  Status set_config_filename(const std::string& filename);

  /**
   * Set the VFS parameters. Note that this has effect immediately, and does
   * not require `init` to be called.
   */
  Status set_vfs_params(const VFSParams& vfs_params);

  /** Unsets a parameter. */
  Status unset(const std::string& param);

 private:
  /* ********************************* */
  /*         PRIVATE ATTRIBUTES        */
  /* ********************************* */

  /** The name of the filename the config parameters will be read from. */
  std::string config_filename_;

  /** Stores a map of param -> value. */
  std::unordered_map<std::string, std::string> param_values_;

  /** The storage manager parameters. */
  SMParams sm_params_;

  /** The VFS parameters. */
  VFSParams vfs_params_;

  /* ********************************* */
  /*          PRIVATE CONSTANTS        */
  /* ********************************* */

  /** Character indicating the start of a comment in a config file. */
  static const char COMMENT_START;

  /* ********************************* */
  /*          PRIVATE METHODS          */
  /* ********************************* */

  /** Sets the default storage manager parameters. */
  void set_default_sm_params();

  /** Sets the default VFS parameters. */
  void set_default_vfs_params();

  /** Sets the default S3 parameters. */
  void set_default_vfs_s3_params();

  /** Sets the config parameters from a configuration file. */
  Status set_from_file();

  /** Sets the array metadata cache size, properly parsing the input value. */
  Status set_sm_array_schema_cache_size(const std::string& value);

  /** Sets the fragment metadata cache size, properly parsing the input value.*/
  Status set_sm_fragment_metadata_cache_size(const std::string& value);

  /** Sets the tile cache size, properly parsing the input value. */
  Status set_sm_tile_cache_size(const std::string& value);

  /** Sets the S3 region. */
  Status set_vfs_s3_region(const std::string& value);

  /** Sets the S3 scheme. */
  Status set_vfs_s3_scheme(const std::string& value);

  /** Sets the S3 endpoint override. */
  Status set_vfs_s3_endpoint_override(const std::string& value);

  /** Sets the S3 virtual addressing. */
  Status set_vfs_s3_use_virtual_addressing(const std::string& value);

  /** Sets the S3 file buffer size. */
  Status set_vfs_s3_file_buffer_size(const std::string& value);

  /** Sets the S3 connect timeout in milliseconds. */
  Status set_vfs_s3_connect_timeout_ms(const std::string& value);

  /** Sets the S3 request timeout in milliseconds. */
  Status set_vfs_s3_request_timeout_ms(const std::string& value);
};

}  // namespace tiledb

#endif  // TILEDB_CONFIG_H
