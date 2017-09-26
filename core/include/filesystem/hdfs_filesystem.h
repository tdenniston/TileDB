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
Status create_dir(const std::string& path);

// delete the directory with the given path
Status delete_dir(const std::string& path);

// Is the given path a valid directory
bool is_dir(const std::string& path);

#ifdef HAVE_HDFS
bool is_dir(const std::string& path, hdfsFS fs);
#endif

// move the directory to a new path
Status move_dir(const std::string& old_dir, const std::string& new_dir);

// Is the given path a valid file
bool is_file(const std::string& path);

// create a file with the given path
Status create_file(const std::string& path);

// delete a file with the given path
Status delete_file(const std::string& path);

// Read length bytes from file give by path from byte offset offset into pre
// allocated buffer buffer.
Status read_from_file(
    const std::string& path, off_t offset, void* buffer, uint64_t length);

Status read_from_file(const std::string& path, Buffer** buff);

// Write length bytes of buffer to a given path
Status write_to_file(
    const std::string& path, const void* buffer, const uint64_t length);

// List all subdirectories and files for a given path, appending them to paths.
// Ordering does not matter.
Status ls(const std::string& path, std::vector<std::string>* paths);

// List all subfiles (1 level deep) for a given path, appending them to fpaths.
// Ordering does not matter.
Status ls_files(const std::string& path, std::vector<std::string>& fpaths);

// List all subdirectories (1 level deep) for a given path, appending them to
// dpaths.  Ordering does not matter.
Status ls_dirs(const std::string& path, std::vector<std::string>& fpaths);

// File size in bytes for a given path
Status file_size(const std::string& path, uint64_t* nbytes);

}  // namespace hdfs

}  // namespace tiledb

#endif  // TILEDB_FILESYSTEM_HDFS_H
