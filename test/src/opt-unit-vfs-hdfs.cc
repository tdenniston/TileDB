#include "catch.hpp"

#include <iostream>
#include "hdfs_filesystem.h"
#include "status.h"
#include "vfs.h"

using namespace tiledb;

TEST_CASE("Test VFS: HDFS URI", "[vfs]") {
  Status st;
  VFS* vfs = new VFS();
  URI tiledb_uri = URI("hdfs:///tiledb_test");
  st = vfs->create_dir(tiledb_uri);
  CHECK(st.ok());
  CHECK(vfs->is_dir(tiledb_uri));
  //st = hdfs::delete_dir(tiledb_uri.to_path());
  //CHECK(st.ok());
  delete vfs;
}
