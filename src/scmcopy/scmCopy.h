/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file scmCopy.h
 * @author drose
 * @date 2000-10-31
 */

#ifndef SCMCOPY_H
#define SCMCOPY_H

#include "pandatoolbase.h"

#include "scmSourceTree.h"

#include "programBase.h"
#include "filename.h"
#include "pvector.h"

/**
 * This is the base class for a family of programs that copy files, typically
 * model files like .flt files and their associated textures, into a SCM-
 * controlled source tree.
 */
class SCMCopy : public ProgramBase {
public:
  SCMCopy();

  SCMSourceTree::FilePath
  import(const Filename &source, void *extra_data,
         SCMSourceDirectory *suggested_dir);

  bool continue_after_error();

protected:
  static bool dispatch_scm_type(const std::string &opt, const std::string &arg, void *var);

  virtual bool handle_args(Args &args);
  virtual bool post_command_line();

  virtual bool verify_file(const Filename &source, const Filename &dest,
                           SCMSourceDirectory *dest_dir,
                           void *extra_data);
  virtual bool copy_file(const Filename &source, const Filename &dest,
                         SCMSourceDirectory *dest_dir,
                         void *extra_data, bool new_file)=0;

  bool verify_binary_file(Filename source, Filename dest);
  bool copy_binary_file(Filename source, Filename dest);

  bool scm_add(const Filename &filename);
  static std::string protect_from_shell(const std::string &source);

  virtual std::string filter_filename(const std::string &source);

private:
  bool scan_hierarchy();
  bool scan_for_root(const std::string &dirname);
  std::string prompt(const std::string &message);

protected:
  // This type of source control manager being used.
  enum Type {
    T_cvs,
    T_git,
  };

  bool _force;
  bool _interactive;
  bool _got_model_dirname;
  Filename _model_dirname;
  bool _got_map_dirname;
  Filename _map_dirname;
  bool _got_root_dirname;
  Filename _root_dirname;
  Filename _key_filename;
  bool _no_scm;
  std::string _scm_binary;
  bool _got_scm_binary;
  bool _user_aborted;
  Type _scm_type;

  typedef pvector<Filename> SourceFiles;
  SourceFiles _source_files;

  SCMSourceTree _tree;
  SCMSourceDirectory *_model_dir;
  SCMSourceDirectory *_map_dir;

  typedef pmap<std::string, SCMSourceTree::FilePath> CopiedFiles;
  CopiedFiles _copied_files;
};

#endif
