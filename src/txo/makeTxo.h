/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file makeTxo.h
 * @author lachbr
 * @date 2020-10-20
 */

#ifndef MAKETXO_H
#define MAKETXO_H

#include "pandatoolbase.h"

#include "programBase.h"
#include "withOutputFile.h"
#include "samplerState.h"
#include "texture.h"

class MakeTxo : public ProgramBase, public WithOutputFile {
public:
  MakeTxo();

  void run();

  enum Filter {
    F_nearest_neighbor,
    F_bilinear,
    F_trilinear,
    F_anisotropic,
  };

  enum Wrap {
    W_repeat,
    W_clamp,
    W_mirror,
    W_border,
  };

  enum Type {
    T_2d,
    T_2d_array,
    T_cube_map,
    T_3d,
  };

protected:
  virtual bool handle_args(Args &args);

private:
  static bool dispatch_type(const std::string &opt, const std::string &arg, void *var);
  static bool dispatch_filter(const std::string &opt, const std::string &arg, void *var);
  static bool dispatch_wrap(const std::string &opt, const std::string &arg, void *var);
  static bool dispatch_compression(const std::string &opt, const std::string &arg, void *var);
  static bool dispatch_auto_scale(const std::string &opt, const std::string &arg, void *var);

private:
  bool _srgb;
  bool _strip_alpha;
  bool _mipmap;
  int _anisotropic_degree;
  int _x;
  bool _got_x;
  int _y;
  bool _got_y;
  double _scale;
  bool _got_scale;
  Filter _filter_mode;
  Wrap _wrap_mode;
  Type _type;
  Texture::CompressionMode _compression;
  Filename _input_image;
  AutoTextureScale _auto_scale;
};

#endif // MAKETXO_H
