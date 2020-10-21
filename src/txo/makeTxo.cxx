/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file makeTxo.cxx
 * @author lachbr
 * @date 2020-10-20
 */

#include "makeTxo.h"
#include "texture.h"
#include "texturePool.h"
#include "loaderOptions.h"
#include "luse.h"

/**
 *
 */
MakeTxo::
MakeTxo() : WithOutputFile(true, false, true)
{
  _srgb = false;
  _strip_alpha = false;
  _mipmap = false;
  _compression = Texture::CM_off;
  _anisotropic_degree = 1;
  _wrap_mode = W_repeat;
  _filter_mode = F_trilinear;
  _type = T_2d;
  _auto_scale = ATS_none;
  _x = 0;
  _got_x = false;
  _y = 0;
  _got_y = false;
  _scale = 1.0;
  _got_scale = false;

  set_program_brief("convert an image for set of images into a .txo file");
  set_program_description
    ("This program converts an image or set of images into a .txo file, which "
     "is Panda's internal texture format.");

  clear_runlines();
  add_runline("[opts] input.jpg output.txo");
  add_runline("[opts] -o output.txo input.jpg");

  add_option
    ("o", "filename", 0,
     "Specify the filename to which the resulting .txo file will be written.  "
     "If this option is omitted, the last parameter name is taken to be the "
     "name of the output file.",
     &MakeTxo::dispatch_filename, &_got_output_filename, &_output_filename);

  add_option
    ("srgb", "", 0,
     "Specify that the input images are in sRGB space.",
     &MakeTxo::dispatch_true, nullptr, &_srgb);

  add_option
    ("stripalpha", "", 0,
     "Specifies that the alpha channel of the input images should be removed.",
     &MakeTxo::dispatch_true, nullptr, &_strip_alpha);

  add_option
    ("mipmap", "", 0,
     "Specify that the texture should use mipmaps.",
     &MakeTxo::dispatch_true, nullptr, &_mipmap);

  add_option
    ("cubemap", "", 0,
     "Specify that this is a cubemap texture.",
     &MakeTxo::dispatch_type, nullptr, &_type);

  add_option
    ("2d", "", 0,
     "Specify that this is a 2D texture.",
     &MakeTxo::dispatch_type, nullptr, &_type);

  add_option
    ("2darray", "", 0,
     "Specify that this is a 2D array texture.",
     &MakeTxo::dispatch_type, nullptr, &_type);

  add_option
    ("3d", "", 0,
     "Specify that this is a 3D texture.",
     &MakeTxo::dispatch_type, nullptr, &_type);

  add_option
    ("nearest", "", 0,
     "Specif that the texture should use nearest neighbor filtering.",
     &MakeTxo::dispatch_filter, nullptr, this);

  add_option
    ("bilinear", "", 0,
     "Specif that the texture should use bilinear filtering.",
     &MakeTxo::dispatch_filter, nullptr, this);

  add_option
    ("trilinear", "", 0,
     "Specify that the texture should use trilinear filtering.",
     &MakeTxo::dispatch_filter, nullptr, this);

  add_option
    ("aniso", "integer", 0,
     "Specify that the texture should use anisotropic filtering to the "
     "indicated degree.",
     &MakeTxo::dispatch_filter, nullptr, this);

  add_option
    ("repeat", "", 0,
     "Specify that the texture should use repeat wrapping.",
     &MakeTxo::dispatch_wrap, nullptr, &_wrap_mode);

  add_option
    ("clamp", "", 0,
     "Specify that the texture should use clamp wrapping.",
     &MakeTxo::dispatch_wrap, nullptr, &_wrap_mode);

  add_option
    ("mirror", "", 0,
     "Specify that the texture should use mirror wrapping.",
     &MakeTxo::dispatch_wrap, nullptr, &_wrap_mode);

  add_option
    ("border", "", 0,
     "Specify that the texture should use border wrapping.",
     &MakeTxo::dispatch_wrap, nullptr, &_wrap_mode);

  add_option
    ("dxt1", "", 0,
     "Specify that the texture should be compressed using the DXT1 algorithm.  "
     "RGB with optional binary alpha.",
     &MakeTxo::dispatch_compression, nullptr, &_compression);

  add_option
    ("dxt2", "", 0,
     "Specify that the texture should be compressed using the DXT2 algorithm.  "
     "Like DXT3, but assumes premultipled alpha.",
     &MakeTxo::dispatch_compression, nullptr, &_compression);

  add_option
    ("dxt3", "", 0,
     "Specify that the texture should be compressed using the DXT3 algorithm.  "
     "RGB with uncompressed 4-bit alpha.",
     &MakeTxo::dispatch_compression, nullptr, &_compression);

  add_option
    ("dxt4", "", 0,
     "Specify that the texture should be compressed using the DXT4 algorithm.  "
     "Like DXT5, but assumes premultiplied alpha.",
     &MakeTxo::dispatch_compression, nullptr, &_compression);

  add_option
    ("dxt5", "", 0,
     "Specify that the texture should be compressed using the DXT4 algorithm.  "
     "RGB with separately compressed 8-bit alpha.",
     &MakeTxo::dispatch_compression, nullptr, &_compression);

  add_option
    ("pow2", "method", 0,
     "Specify how to rescale the texture to a power-of-two.  "
     "Leave undefined or specify \"none\" to not rescale the texture.",
     &MakeTxo::dispatch_auto_scale, nullptr, &_auto_scale);

  add_option
    ("x", "integer", 0,
     "Set a new X size for the texture.",
     &MakeTxo::dispatch_int, &_got_x, &_x);

  add_option
    ("y", "integer", 0,
     "Set a new Y size for the texture.",
     &MakeTxo::dispatch_int, &_got_y, &_y);

  add_option
    ("scale", "float", 0,
     "Scale the texture dimensions by this amount.",
     &MakeTxo::dispatch_double, &_got_scale, &_scale);
}

/**
 *
 */
void MakeTxo::
run() {
  PT(Texture) tex;

  LoaderOptions opts;
  opts.set_auto_texture_scale(_auto_scale);
  int flags = (
    LoaderOptions::TF_allow_compression |
    LoaderOptions::TF_allow_1d |
    LoaderOptions::TF_preload
  );
  if (_mipmap) {
    flags |= LoaderOptions::TF_generate_mipmaps;
  }
  opts.set_texture_flags(flags);

  switch (_type) {
  default:
  case T_2d:
    tex = TexturePool::load_texture(_input_image, 0, false, opts);
    break;
  case T_cube_map:
    tex = TexturePool::load_cube_map(_input_image, false, opts);
    break;
  case T_2d_array:
    tex = TexturePool::load_2d_texture_array(_input_image, false, opts);
    break;
  case T_3d:
    tex = TexturePool::load_3d_texture(_input_image, false, opts);
    break;
  }

  if (!tex) {
    nout << "Couldn't load the input image!\n";
    return;
  }

  if (_got_x) {
    tex->set_x_size(_x);
  }

  if (_got_y) {
    tex->set_y_size(_y);
  }

  if (_got_scale) {
    tex->set_x_size(tex->get_x_size() * _scale);
    tex->set_y_size(tex->get_y_size() * _scale);
  }

  if (_srgb) {
    switch (tex->get_format()) {
    case Texture::F_rgba:
      tex->set_format(Texture::F_srgb_alpha);
      break;
    case Texture::F_rgb:
      tex->set_format(Texture::F_srgb);
      break;
    case Texture::F_luminance:
      tex->set_format(Texture::F_sluminance);
      break;
    case Texture::F_luminance_alpha:
      tex->set_format(Texture::F_sluminance_alpha);
      break;
    }
  }

  switch (_wrap_mode) {
  default:
  case W_clamp:
    tex->set_wrap_u(SamplerState::WM_clamp);
    tex->set_wrap_v(SamplerState::WM_clamp);
    tex->set_wrap_w(SamplerState::WM_clamp);
    break;
  case W_repeat:
    tex->set_wrap_u(SamplerState::WM_repeat);
    tex->set_wrap_v(SamplerState::WM_repeat);
    tex->set_wrap_w(SamplerState::WM_repeat);
    break;
  case W_mirror:
    tex->set_wrap_u(SamplerState::WM_mirror);
    tex->set_wrap_v(SamplerState::WM_mirror);
    tex->set_wrap_w(SamplerState::WM_mirror);
    break;
  case W_border:
    tex->set_wrap_u(SamplerState::WM_border_color);
    tex->set_wrap_v(SamplerState::WM_border_color);
    tex->set_wrap_w(SamplerState::WM_border_color);
    tex->set_border_color(LColor(0, 0, 0, 1));
    break;
  }

  switch (_filter_mode) {
  case F_nearest_neighbor:
    if (_mipmap) {
      tex->set_minfilter(SamplerState::FT_nearest_mipmap_nearest);
      tex->set_magfilter(SamplerState::FT_nearest);
    } else {
      tex->set_minfilter(SamplerState::FT_nearest);
      tex->set_magfilter(SamplerState::FT_nearest);
    }
    break;
  case F_bilinear:
    if (_mipmap) {
      tex->set_minfilter(SamplerState::FT_linear_mipmap_nearest);
      tex->set_magfilter(SamplerState::FT_linear);
    } else {
      tex->set_minfilter(SamplerState::FT_linear);
      tex->set_magfilter(SamplerState::FT_linear);
    }
    break;
  default:
  case F_trilinear:
  case F_anisotropic:
    if (_mipmap) {
      tex->set_minfilter(SamplerState::FT_linear_mipmap_linear);
      tex->set_magfilter(SamplerState::FT_linear);
    } else {
      tex->set_minfilter(SamplerState::FT_linear);
      tex->set_magfilter(SamplerState::FT_linear);
    }
    break;
  }

  tex->set_anisotropic_degree(_anisotropic_degree);

  tex->set_compression(_compression);

  tex->reload();

  nout << "\nTexture info:\n";
  tex->write(nout, 0);
  nout << "\n";

  std::ostream &out = get_output();
  tex->write_txo(out, _output_filename.get_fullpath());
  close_output();
}

/**
 *
 */
bool MakeTxo::
handle_args(Args &args) {
  if (args.empty()) {
    nout << "You didn't specify an input image!\n";
    return false;
  }

  if (args.size() > (size_t)1) {
    nout << "Specify only one input image on the command line.\n";
    return false;
  }

  _input_image = Filename::from_os_specific(args[0]);

  return true;
}

/**
 *
 */
bool MakeTxo::
dispatch_type(const std::string &opt, const std::string &arg, void *var) {
  if (opt == "cubemap") {
    *(Type *)var = T_cube_map;
  } else if (opt == "2d") {
    *(Type *)var = T_2d;
  } else if (opt == "2darray") {
    *(Type *)var = T_2d_array;
  } else if (opt == "3d") {
    *(Type *)var = T_3d;
  } else {
    return false;
  }

  return true;

}

/**
 *
 */
bool MakeTxo::
dispatch_filter(const std::string &opt, const std::string &arg, void *var) {
  MakeTxo *prog = (MakeTxo *)var;

  if (opt == "nearest") {
    prog->_filter_mode = F_nearest_neighbor;
  } else if (opt == "bilinear") {
    prog->_filter_mode = F_bilinear;
  } else if (opt == "trilinear") {
    prog->_filter_mode = F_trilinear;
  } else if (opt == "aniso") {
    prog->_filter_mode = F_anisotropic;
    if (!string_to_int(arg, prog->_anisotropic_degree)) {
      nout << "Invalid numeric parameter for -" << opt << ": "
           << arg << "\n";
      return false;
    }
  } else {
    return false;
  }

  return true;
}

/**
 *
 */
bool MakeTxo::
dispatch_wrap(const std::string &opt, const std::string &arg, void *var) {
  if (opt == "repeat") {
    *(Wrap *)var = W_repeat;
  } else if (opt == "clamp") {
    *(Wrap *)var = W_clamp;
  } else if (opt == "mirror") {
    *(Wrap *)var = W_mirror;
  } else if (opt == "border") {
    *(Wrap *)var = W_border;
  } else {
    return false;
  }

  return true;
}

/**
 *
 */
bool MakeTxo::
dispatch_compression(const std::string &opt, const std::string &arg, void *var) {
  if (opt == "dxt1") {
    *(Texture::CompressionMode *)var = Texture::CM_dxt1;
  } else if (opt == "dxt2") {
    *(Texture::CompressionMode *)var = Texture::CM_dxt2;
  } else if (opt == "dxt3") {
    *(Texture::CompressionMode *)var = Texture::CM_dxt3;
  } else if (opt == "dxt4") {
    *(Texture::CompressionMode *)var = Texture::CM_dxt4;
  } else if (opt == "dxt5") {
    *(Texture::CompressionMode *)var = Texture::CM_dxt5;
  } else {
    return false;
  }

  return true;
}

/**
 *
 */
bool MakeTxo::
dispatch_auto_scale(const std::string &opt, const std::string &arg, void *var) {
  if (arg == "none") {
    *(AutoTextureScale *)var = ATS_none;
  } else if (arg == "down") {
    *(AutoTextureScale *)var = ATS_down;
  } else if (arg == "up") {
    *(AutoTextureScale *)var = ATS_up;
  } else if (arg == "pad") {
    *(AutoTextureScale *)var = ATS_pad;
  } else {
    nout << "Invalid argument to -" << opt << ": " << arg << "\n";
    return false;
  }

  return true;
}

int
main(int argc, char *argv[]) {
  MakeTxo prog;
  prog.parse_command_line(argc, argv);
  prog.run();
  return 0;
}
