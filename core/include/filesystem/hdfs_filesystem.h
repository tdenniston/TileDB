/**
 * @file   hdfs_filesystem.h
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
 * This file includes declarations of HDFS filesystem functions.
 */

#ifndef TILEDB_FILESYSTEM_HDFS_H
#define TILEDB_FILESYSTEM_HDFS_H

#include <sys/types.h>
#include <string>
#include <vector>

#include "buffer.h"
#include "status.h"
#include "uri.h"

#ifdef HAVE_HDFS
#include "hdfs.h"
#endif

namespace tiledb {

namespace hdfs {

#ifdef HAVE_HDFS
// create a connection to hdfs
Status connect(hdfsFS& fs);

// disconnect hdfs connection
Status disconnect(hdfsFS& fs);
#endif

// create a directory with the given path
Status create_dir(hdfsFS fs, const URI& uri);

// delete the directory with the given path
Status delete_dir(hdfsFS fs, const URI& uri);

// Is the given path a valid directory
bool is_dir(hdfsFS fs, const URI& uri);

// move the directory to a new path
Status move_dir(hdfsFS fs, const URI& old_uri, const URI& new_uri);

// Is the given path a valid file
bool is_file(hdfsFS fs, const URI& uri);

// create a file with the given path
Status create_file(hdfsFS fs, const URI& uri);

// delete a file with the given path
Status delete_file(hdfsFS fs, const URI& uri);

// Read length bytes from file give by path from byte offset offset into pre
// allocated buffer buffer.
Status read_from_file(
    hdfsFS fs, const URI& uri, off_t offset, void* buffer, uint64_t length);

Status read_from_file(hdfsFS fs, const URI& path, Buffer** buff);

// Write length bytes of buffer to a given path
Status write_to_file(
    hdfsFS fs, const URI& uri, const void* buffer, const uint64_t length);

// List all subdirectories and files for a given path, appending them to paths.
// Ordering does not matter.
Status ls(hdfsFS fs, const URI& uri, std::vector<std::string>* paths);

// List all subfiles (1 level deep) for a given path, appending them to fpaths.
// Ordering does not matter.
Status ls_files(hdfsFS fs, const URI& uri, std::vector<std::string>& fpaths);

// List all subdirectories (1 level deep) for a given path, appending them to
// dpaths.  Ordering does not matter.
Status ls_dirs(hdfsFS fs, const URI& uri, std::vector<std::string>& fpaths);

// File size in bytes for a given path
Status file_size(hdfsFS fs, const URI& uri, uint64_t* nbytes);

}  // namespace hdfs

}  // namespace tiledb

#endif  // TILEDB_FILESYSTEM_HDFS_H
