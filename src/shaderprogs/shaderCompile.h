/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file shaderCompile.h
 * @author brian
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
  static bool dispatch_stage(const std::string &opt, const std::string &arg, void *var);

  class VariationData {
  public:
    VariationData() = default;

    ShaderObject::VariationBuilder builder;
    bool skip;
  };

public:
  ShaderCompiler::Options *_curr_options;

  bool _verbose;
  PT(ShaderObject) _sho;
  ShaderModule::Stage _stage;
  int _num_threads;
  int _num_skipped;
  Filename _input_filename;
  pvector<VariationData> _variations;
  vector_int _non_skipped_variations;
};

#endif // SHADERCOMPILE_H
