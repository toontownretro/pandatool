// Filename: txaFile.cxx
// Created by:  drose (30Nov00)
// 
////////////////////////////////////////////////////////////////////

#include "txaFile.h"
#include "string_utils.h"
#include "palettizer.h"
#include "paletteGroup.h"
#include "textureImage.h"

#include <notify.h>
#include <pnmFileTypeRegistry.h>

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::Constructor
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
TxaFile::
TxaFile() {
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::read
//       Access: Public
//  Description: Reads the indicated .txa filename, and returns true
//               if successful, or false if there is an error.
////////////////////////////////////////////////////////////////////
bool TxaFile::
read(Filename filename) {
  filename.set_text();
  ifstream in;
  if (!filename.open_read(in)) {
    nout << "Unable to open " << filename << "\n";
    return false;
  }

  string line;
  getline(in, line);
  int line_number = 1;

  while (!in.eof() && !in.fail()) {
    bool okflag = true;

    // Strip off the comment.
    size_t hash = line.find('#');
    if (hash != string::npos) {
      line = line.substr(0, hash);
    }
    line = trim_left(line);
    if (line.empty()) {
      // Empty lines are ignored.

    } else if (line[0] == ':') {
      // This is a keyword line.
      vector_string words;
      extract_words(line, words);
      if (words[0] == ":group") {
	okflag = parse_group_line(words);

      } else if (words[0] == ":palette") {
	okflag = parse_palette_line(words);

      } else if (words[0] == ":margin") {
	okflag = parse_margin_line(words);

      } else if (words[0] == ":repeat") {
	okflag = parse_repeat_line(words);

      } else if (words[0] == ":imagetype") {
	okflag = parse_imagetype_line(words);

      } else if (words[0] == ":round") {
	okflag = parse_round_line(words);

      } else if (words[0] == ":remap") {
	okflag = parse_remap_line(words);

      } else {
	nout << "Invalid keyword " << words[0] << "\n";
	okflag = false;
      }

    } else {
      _lines.push_back(TxaLine());
      TxaLine &txa_line = _lines.back();

      okflag = txa_line.parse(line);
    }

    if (!okflag) {
      nout << "Error on line " << line_number << " of " << filename << "\n";
      return false;
    }

    getline(in, line);
    line_number++;
  }

  if (!in.eof()) {
    nout << "I/O error reading " << filename << "\n";
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::match_egg
//       Access: Public
//  Description: Searches for a matching line in the .txa file for the
//               given egg file and applies its specifications.  If a
//               match is found, returns true; otherwise, returns
//               false.  Also returns false if all the matching lines
//               for the egg file include the keyword "cont".
////////////////////////////////////////////////////////////////////
bool TxaFile::
match_egg(EggFile *egg_file) const {
  Lines::const_iterator li;
  for (li = _lines.begin(); li != _lines.end(); ++li) {
    if ((*li).match_egg(egg_file)) {
      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::match_texture
//       Access: Public
//  Description: Searches for a matching line in the .txa file for the
//               given texture and applies its specifications.  If a
//               match is found, returns true; otherwise, returns
//               false.  Also returns false if all the matching lines
//               for the texture include the keyword "cont".
////////////////////////////////////////////////////////////////////
bool TxaFile::
match_texture(TextureImage *texture) const {
  Lines::const_iterator li;
  for (li = _lines.begin(); li != _lines.end(); ++li) {
    if ((*li).match_texture(texture)) {
      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::write
//       Access: Public
//  Description: Outputs a representation of the lines that were read
//               in to the indicated output stream.  This is primarily
//               useful for debugging.
////////////////////////////////////////////////////////////////////
void TxaFile::
write(ostream &out) const {
  Lines::const_iterator li;
  for (li = _lines.begin(); li != _lines.end(); ++li) {
    out << (*li) << "\n";
  }
}


////////////////////////////////////////////////////////////////////
//     Function: TxaFile::parse_group_line
//       Access: Private
//  Description: Handles the line in a .txa file that begins with the
//               keyword ":group" and indicates the relationships
//               between one or more groups.
////////////////////////////////////////////////////////////////////
bool TxaFile::
parse_group_line(const vector_string &words) {
  vector_string::const_iterator wi;
  wi = words.begin();
  assert (wi != words.end());
  ++wi;

  const string &group_name = (*wi);
  PaletteGroup *group = pal->get_palette_group(group_name);
  ++wi;

  enum State {
    S_none,
    S_with,
    S_dir,
  };
  State state = S_none;

  while (wi != words.end()) {
    const string &word = (*wi);
    if (word == "with") {
      state = S_with;

    } else if (word == "dir") {
      state = S_dir;

    } else {
      switch (state) {
      case S_none:
	nout << "Invalid keyword: " << word << "\n";
	return false;

      case S_with:
	group->group_with(pal->get_palette_group(word));
	break;

      case S_dir:
	group->set_dirname(word);
	state = S_none;
	break;
      }
    }

    ++wi;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::parse_palette_line
//       Access: Private
//  Description: Handles the line in a .txa file that begins with the
//               keyword ":palette" and indicates the appropriate size
//               for the palette images.
////////////////////////////////////////////////////////////////////
bool TxaFile::
parse_palette_line(const vector_string &words) {
  if (words.size() != 3) {
    nout << "Exactly two parameters required for :palette, the x and y "
	 << "size of the palette images to generate.\n";
    return false;
  }

  if (!string_to_int(words[1], pal->_pal_x_size) ||
      !string_to_int(words[2], pal->_pal_y_size)) {
    nout << "Invalid palette size: " << words[1] << " " << words[2] << "\n";
    return false;
  }

  if (pal->_pal_x_size <= 0 || pal->_pal_y_size <= 0) {
    nout << "Invalid palette size: " << pal->_pal_x_size 
	 << " " << pal->_pal_y_size << "\n";
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::parse_margin_line
//       Access: Private
//  Description: Handles the line in a .txa file that begins with the
//               keyword ":margin" and indicates the default margin
//               size.
////////////////////////////////////////////////////////////////////
bool TxaFile::
parse_margin_line(const vector_string &words) {
  if (words.size() != 2) {
    nout << "Exactly one parameter required for :margin, the "
	 << "size of the default margin to apply.\n";
    return false;
  }

  if (!string_to_int(words[1], pal->_margin)) {
    nout << "Invalid margin: " << words[1] << "\n";
    return false;
  }

  if (pal->_margin < 0) {
    nout << "Invalid margin: " << pal->_margin << "\n";
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::parse_repeat_line
//       Access: Private
//  Description: Handles the line in a .txa file that begins with the
//               keyword ":repeat" and indicates the default repeat
//               threshold.
////////////////////////////////////////////////////////////////////
bool TxaFile::
parse_repeat_line(const vector_string &words) {
  if (words.size() != 2) {
    nout << "Exactly one parameter required for :repeat, the "
	 << "percentage for the default repeat threshold.\n";
    return false;
  }

  string tail;
  pal->_repeat_threshold = string_to_double(words[1], tail);
  if (!(tail.empty() || tail == "%")) {
    // This is an invalid number.
    nout << "Invalid repeat threshold: " << words[1] << "\n";
    return false;
  }

  if (pal->_repeat_threshold <= 0.0) {
    nout << "Invalid repeat threshold: " << pal->_repeat_threshold << "\n";
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::parse_imagetype_line
//       Access: Private
//  Description: Handles the line in a .txa file that begins with the
//               keyword ":imagetype" and indicates the default image
//               file type to convert palettes and textures to.
////////////////////////////////////////////////////////////////////
bool TxaFile::
parse_imagetype_line(const vector_string &words) {
  if (words.size() != 2) {
    nout << "Exactly one parameter required for :imagetype.\n";
    return false;
  }
  const string &imagetype = words[1];
  if (!parse_image_type_request(imagetype, pal->_color_type, pal->_alpha_type)) {
    nout << "\nKnown image types are:\n";
    PNMFileTypeRegistry::get_ptr()->write_types(nout, 2);
    nout << "\n";
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::parse_round_line
//       Access: Private
//  Description: Handles the line in a .txa file that begins with the
//               keyword ":round" and indicates how or whether to
//               round up UV minmax boxes.
////////////////////////////////////////////////////////////////////
bool TxaFile::
parse_round_line(const vector_string &words) {
  if (words.size() == 2) {
    if (words[1] == "no") {
      pal->_round_uvs = false;
      return true;
    } else {
      nout << "Invalid round keyword: " << words[1] << ".\n"
	   << "Expected 'no', or a round fraction and fuzz factor.\n";
      return false;
    }
  }

  if (words.size() != 3) {
    nout << "Exactly two parameters required for :round, the fraction "
	 << "to round to, and the fuzz factor.\n";
    return false;
  }

  if (!string_to_double(words[1], pal->_round_unit) ||
      !string_to_double(words[2], pal->_round_fuzz)) {
    nout << "Invalid rounding: " << words[1] << " " << words[2] << "\n";
    return false;
  }

  if (pal->_round_unit <= 0.0 || pal->_round_fuzz < 0.0) {
    nout << "Invalid rounding: " << pal->_round_unit 
	 << " " << pal->_round_fuzz << "\n";
    return false;
  }

  pal->_round_uvs = true;
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: TxaFile::parse_remap_line
//       Access: Private
//  Description: Handles the line in a .txa file that begins with the
//               keyword ":remap" and indicates how or whether to
//               remap UV coordinates in egg files to the unit box.
////////////////////////////////////////////////////////////////////
bool TxaFile::
parse_remap_line(const vector_string &words) {
  if (words.size() != 2) {
    nout << "Exactly one parameter required for :remap.\n";
    return false;
  }

  const string &keyword = words[1];
  if (keyword == "never") {
    pal->_remap_uv = Palettizer::RU_never;

  } else if (keyword == "group") {
    pal->_remap_uv = Palettizer::RU_group;

  } else if (keyword == "poly") {
    pal->_remap_uv = Palettizer::RU_poly;

  } else {
    nout << "Invalid remap keyword: " << keyword << "\n";
    return false;
  }

  return true;
}