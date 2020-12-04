/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file scmSourceDirectory.h
 * @author drose
 * @date 2000-10-31
 */

#ifndef SCMSOURCEDIRECTORY_H
#define SCMSOURCEDIRECTORY_H

#include "pandatoolbase.h"
#include "filename.h"

#include "pvector.h"

class SCMSourceTree;

/**
 * This represents one particular directory in the hierarchy of source
 * directory files.  We must scan the source directory to identify where the
 * related files have previously been copied.
 *
 * The tree is maintained in a case-insensitive manner, even on a non-Windows
 * system, since you might want to eventually check out the SCM tree onto a
 * Windows system--and if you do, you'll be sad if there are case conflicts
 * within the tree.  So we make an effort to ensure this doesn't happen by
 * treating two files with a different case as the same file.
 */
class SCMSourceDirectory {
public:
  SCMSourceDirectory(SCMSourceTree *tree, SCMSourceDirectory *parent,
                     const std::string &dirname);
  ~SCMSourceDirectory();

  std::string get_dirname() const;
  Filename get_fullpath() const;
  Filename get_path() const;
  Filename get_rel_to(const SCMSourceDirectory *other) const;

  int get_num_children() const;
  SCMSourceDirectory *get_child(int n) const;

  SCMSourceDirectory *find_relpath(const std::string &relpath);
  SCMSourceDirectory *find_dirname(const std::string &dirname);

public:
  bool scan(const Filename &directory, const std::string &key_filename);

private:
  SCMSourceTree *_tree;
  SCMSourceDirectory *_parent;
  std::string _dirname;
  int _depth;

  typedef pvector<SCMSourceDirectory *> Children;
  Children _children;
};

#endif
