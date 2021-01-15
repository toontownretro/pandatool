/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file textureProperties.h
 * @author drose
 * @date 2000-11-28
 */

#ifndef TEXTUREPROPERTIES_H
#define TEXTUREPROPERTIES_H

#include "pandatoolbase.h"

#include "typedWritable.h"

class PNMFileType;
class FactoryParams;

/**
 * This is the set of characteristics of a texture that, if different from
 * another texture, prevent the two textures from sharing a PaletteImage.  It
 * includes properties such as mipmapping, number of channels, etc.
 */
class TextureProperties : public TypedWritable {
public:
  enum Format {
    F_unspecified,
    F_rgba, F_rgbm, F_rgba12, F_rgba8, F_rgba4, F_rgba5,
    F_rgb, F_rgb12, F_rgb8, F_rgb5, F_rgb332,
    F_red, F_green, F_blue, F_alpha, F_luminance,
    F_luminance_alpha, F_luminance_alphamask,
    F_srgb, F_srgb_alpha,
    F_sluminance, F_sluminance_alpha
  };

  enum WrapMode {
    WM_unspecified, WM_clamp, WM_repeat,
    WM_mirror, WM_mirror_once, WM_border_color
  };

  enum FilterType {
    // Note that these type values match up, name-for-name, with a similar
    // enumerated type in Panda's Texture object.  However, they do *not*
    // match up numerically.  You must convert between them using a switch
    // statement.
    FT_unspecified,

    // Mag Filter and Min Filter
    FT_nearest,
    FT_linear,

    // Min Filter Only
    FT_nearest_mipmap_nearest,   // "mipmap point"
    FT_linear_mipmap_nearest,    // "mipmap linear"
    FT_nearest_mipmap_linear,    // "mipmap bilinear"
    FT_linear_mipmap_linear,     // "mipmap trilinear"
  };

  enum QualityLevel {
    QL_unspecified,
    QL_default,
    QL_fastest,
    QL_normal,
    QL_best,
  };

  TextureProperties();
  TextureProperties(const TextureProperties &copy);
  void operator = (const TextureProperties &copy);

  void clear_basic();

  bool has_num_channels() const;
  int get_num_channels() const;
  void set_num_channels(int num_channels);
  void force_grayscale();
  void force_nonalpha();
  bool uses_alpha() const;

  std::string get_string() const;
  void update_properties(const TextureProperties &other);
  void fully_define();

  void update_egg_tex(EggTexture *egg_tex) const;
  bool egg_properties_match(const TextureProperties &other) const;

  bool operator < (const TextureProperties &other) const;
  bool operator == (const TextureProperties &other) const;
  bool operator != (const TextureProperties &other) const;

  Format _format;
  bool _force_format;  // true when format has been explicitly specified
  bool _generic_format; // true if 'generic' keyword, meaning rgba8 -> rgba.
  bool _keep_format;   // true if 'keep-format' keyword.
  FilterType _minfilter, _magfilter;
  QualityLevel _quality_level;
  int _anisotropic_degree;
  PNMFileType *_color_type;
  PNMFileType *_alpha_type;
  bool _srgb;

private:
  static std::string get_format_string(Format format);
  static std::string get_filter_string(FilterType filter_type);
  static std::string get_anisotropic_degree_string(int aniso_degree);
  static std::string get_quality_level_string(QualityLevel quality_level);
  static std::string get_type_string(PNMFileType *color_type,
                                PNMFileType *alpha_type);

  static Format union_format(Format a,
                             Format b);

  static FilterType union_filter(FilterType a,
                                 FilterType b);
  static QualityLevel union_quality_level(QualityLevel a,
                                          QualityLevel b);

  bool _got_num_channels;
  int _num_channels;
  int _effective_num_channels;

  // The TypedWritable interface follows.
public:
  static void register_with_read_factory();
  virtual void write_datagram(BamWriter *writer, Datagram &datagram);
  virtual int complete_pointers(TypedWritable **p_list,
                                BamReader *manager);

protected:
  static TypedWritable *make_TextureProperties(const FactoryParams &params);

public:
  void fillin(DatagramIterator &scan, BamReader *manager);

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedWritable::init_type();
    register_type(_type_handle, "TextureProperties",
                  TypedWritable::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }

private:
  static TypeHandle _type_handle;
};

#endif
