/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file shaderCompile.h
 * @author lachbr
 * @date 2020-12-22
 */

#ifndef SHADERCOMPILE_H
#define SHADERCOMPILE_H

#include "pandatoolbase.h"
#include "programBase.h"
#include "withOutputFile.h"
#include "shaderModule.h"
#include "shaderCompiler.h"
#include "shaderObject.h"
#include "thread.h"

/**
 * Program that compiles a raw shader source file into a shader object.  Each
 * possible combo variation of the shader is compiled and stored inside the
 * shader object.
 */
class ShaderCompile : public ProgramBase, public WithOutputFile {
public:
  ShaderCompile();

  bool run();

  void compile_variation(int n);

protected:
  virtual bool handle_args(Args &args);

private:
  class SkipCommand {
  public:
    enum Command {
      C_and,
      C_or,
      C_not,
      C_ref,
      C_eq,
      C_neq,
      C_literal,
    };

    int eval();

    Command cmd;
    pvector<SkipCommand> arguments;
    int value;
    CPT_InternalName name;
  };

  void collect_combos();
  SkipCommand r_expand_expression(const std::string &str, size_t p);
  SkipCommand r_expand_command(const std::string &str, size_t &vp);
  std::string r_scan_variable(const std::string &str, size_t &vp);
  void tokenize_params(const std::string &str, vector_string &tokens,
                       bool expand);

  static bool dispatch_stage(const std::string &opt, const std::string &arg, void *var);

  class VariationData {
  public:
    VariationData() = default;

    ShaderCompiler::Options options;
    bool skip;
  };

public:
  ShaderCompiler::Options *_curr_options;

  bool _verbose;
  std::string _shader_source;
  PT(VirtualFile) _source_vf;
  PT(ShaderObject) _sho;
  ShaderModule::Stage _stage;
  int _num_threads;
  int _num_skipped;
  Filename _input_filename;
  pvector<VariationData> _variations;
  pvector<SkipCommand> _skip_commands;
  vector_int _non_skipped_variations;
};

#endif // SHADERCOMPILE_H
