// Filename: eggPalettize.cxx
// Created by:  drose (28Nov00)
// 
////////////////////////////////////////////////////////////////////

#include "eggPalettize.h"
#include "palettizer.h"
#include "eggFile.h"
#include "string_utils.h"

#include <eggData.h>
#include <bamFile.h>

#include <stdio.h>

////////////////////////////////////////////////////////////////////
//     Function: EggPalettize::Constructor
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
EggPalettize::
EggPalettize() : EggMultiFilter(true) {
  set_program_description
    ("egg-palettize attempts to pack several texture maps from various models "
     "together into one or more palette images, for improved rendering performance "
     "and ease of texture management.  It can also resize textures on the fly, "
     "whether or not they are actually placed on a palette.\n\n"
     
     "egg-palettize reads and writes an AttributesFile, which contains instructions "
     "from the user about resizing particular textures, as well as the complete "
     "information necessary to reconstruct the palettization from past runs, "
     "including references to other egg files that may share this palette.  This "
     "is designed to allow multiple egg files to use the same palette, without "
     "having to process them all at once.\n\n"
     
     "Note that it is not even necessary to specify any egg files at all on the "
     "command line; egg-palettize can be run on an existing AttributesFiles by "
     "itself to freshen up a palette when necessary.");


  clear_runlines();
  add_runline("[opts] attribfile.txa file.egg [file.egg ...]");

  // We always have EggMultiBase's -f on: force complete load.  In
  // fact, we use -f for our own purposes, below.
  remove_option("f");
  _force_complete = true;

  add_option
    ("a", "filename", 0, 
     "Read the indicated file as the .txa file.  The default is textures.txa.",
     &EggPalettize::dispatch_filename, NULL, &_txa_filename);

  add_option
    ("pi", "", 0, 
     "Do not process anything, but instead report the detailed palettization "
     "information.",
     &EggPalettize::dispatch_none, &_report_pi);

  add_option
    ("s", "", 0, 
     "Do not process anything, but report statistics on palette "
     "and texture itilization.",
     &EggPalettize::dispatch_none, &_statistics_only);

  // We redefine -d using add_option() instead of redescribe_option()
  // so it gets listed along with these other options that relate.
  add_option
    ("d", "dirname", 0, 
     "The directory in which to write the palettized egg files.  This is "
     "only necessary if more than one egg file is processed at the same "
     "time; if it is included, each egg file will be processed and written "
     "into the indicated directory.",
     &EggPalettize::dispatch_filename, &_got_output_dirname, &_output_dirname);
  add_option
    ("dm", "dirname", 0, 
     "The directory in which to place all maps: generated palettes, "
     "as well as images which were not placed on palettes "
     "(but may have been resized).  If this contains the string %s, "
     "this will be replaced with the \"dir\" string associated with a "
     "palette group.",
     &EggPalettize::dispatch_string, &_got_map_dirname, &_map_dirname);
  add_option
    ("dr", "dirname", 0, 
     "The directory to make map filenames relative to when writing egg "
     "files.  If specified, this should be an initial substring of -dm.",
     &EggPalettize::dispatch_filename, &_got_rel_dirname, &_rel_dirname);
  add_option
    ("g", "group", 0, 
     "The default palette group that egg files will be assigned to if they "
     "are not explicitly assigned to any other group.",
     &EggPalettize::dispatch_string, &_got_default_groupname, &_default_groupname);
  add_option
    ("gdir", "name", 0, 
     "The \"dir\" string to associate with the default palette group "
     "specified with -g, if no other dir name is given in the .txa file.",
     &EggPalettize::dispatch_string, &_got_default_groupdir, &_default_groupdir);
  
  add_option
    ("f", "", 0, 
     "Force an optimal packing.  By default, textures are added to "
     "existing palettes without disturbing them, which can lead to "
     "suboptimal packing.  Including this switch forces the palettes "
     "to be rebuilt if necessary to optimize the packing, but this "
     "may invalidate other egg files which share this palette.",
     &EggPalettize::dispatch_none, &_force_optimal);
  add_option
    ("F", "", 0, 
     "Force a redo of everything.  This is useful in case something "
     "has gotten out of sync and the old palettes are just bad.",
     &EggPalettize::dispatch_none, &_force_redo_all);
  add_option
    ("R", "", 0, 
     "Resize mostly-empty palettes to their minimal size.",
     &EggPalettize::dispatch_none, &_optimal_resize);
  add_option
    ("t", "", 0, 
     "Touch any additional egg files that share this palette and will "
     "need to be refreshed, but were not included on the command "
     "line.  Presumably a future make pass will cause them to be run "
     "through egg-palettize again.",
     &EggPalettize::dispatch_none, &_touch_eggs);

  add_option
    ("nolock", "", 0, 
     "Don't attempt to lock the .pi file before rewriting it.  Use "
     "with extreme caution, as multiple processes running on the same "
     ".pi file may overwrite each other.  Use this only if the lock "
     "cannot be achieved for some reason.",
     &EggPalettize::dispatch_none, &_dont_lock_pi);
  add_option
    ("H", "", 0, 
     "Describe the syntax of the attributes file.",
     &EggPalettize::dispatch_none, &_describe_input_file);

  _txa_filename = "textures.txa";
}


////////////////////////////////////////////////////////////////////
//     Function: EggPalettize::handle_args
//       Access: Protected, Virtual
//  Description: Does something with the additional arguments on the
//               command line (after all the -options have been
//               parsed).  Returns true if the arguments are good,
//               false otherwise.
////////////////////////////////////////////////////////////////////
bool EggPalettize::
handle_args(ProgramBase::Args &args) {
  if (_describe_input_file) {
    describe_input_file();
    exit(1);
  }

  return EggMultiFilter::handle_args(args);
}

////////////////////////////////////////////////////////////////////
//     Function: EggPalettize::describe_input_file
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
void EggPalettize::
describe_input_file() {
  nout <<
    "An attributes file consists mostly of lines describing desired sizes of "
    "texture maps.  The format resembles, but is not identical to, that of "
    "the qtess input file.  Examples:\n\n"

    "  texturename.rgb : 64 64\n"
    "  texture-a.rgb texture-b.rgb : 32 16 2\n"
    "  *.rgb : 50%\n"
    "  eyelids.rgb : 16 16 omit\n\n"

    "In general, each line consists of one or more filenames (and can "
    "contain shell globbing characters like '*' or '?'), and a colon "
    "followed by a size request.  For each texture appearing in an egg "
    "file, the input list is scanned from the beginning and the first "
    "line that matches the filename defines the size of the texture.\n\n"

    "A size request may be either a pair of numbers, giving a specific x y "
    "size of the texture, or it may be a scale factor in the form of a "
    "percentage.  It may also include an additional number, giving a margin "
    "for this particular texture (otherwise the default margin is "
    "applied).  Finally, the keyword 'omit' may be included along with the "
    "size to specify that the texture should not be placed in a palette.\n\n"

    "The attributes file may also assign certain egg files into various "
    "named palette groups.  The syntax is similar to the above:\n\n"

    "  car-blue.egg : main\n"
    "  road.egg house.egg : main\n"
    "  plane.egg : phase2 main\n"
    "  car*.egg : phase2\n\n"

    "Any number of egg files may be named on one line, and the group of "
    "egg files may be simultaneously assigned to one or more groups.  Each "
    "named group represents a semi-independent collection of textures; a "
    "different set of palette images will be created for each group.  Each "
    "texture that is referenced by a given egg file will be palettized "
    "in one of the groups assigned to the egg file.  Also see the "
    ":group command, below, which defines relationships between the "
    "different groups.\n\n"

    "There are some other special lines that may appear in this second, "
    "along with the resize commands.  They begin with a colon to "
    "distinguish them from the resize commands.  They are:\n\n"

    "  :palette xsize ysize\n\n"

    "This specifies the size of the palette file(s) to be created.  It "
    "overrides the -s command-line option.\n\n"

    "  :margin msize\n\n"

    "This specifies the size of the default margin for all subsequent "
    "resize commands.  This may appear several times in a given file.\n\n"

    "  :group groupname1 with groupname2 [groupname3 ...]\n\n"

    "This indicates that the palette group named by groupname1 should "
    "be allowed to shared textures with those on groupname2 or groupname3, "
    "etc.  In other words, that whenever palette group groupname1 is in "
    "texture memory, we can assume that palette groups groupname2 and "
    "groupname3 will also be in memory.  Textures that already exist on "
    "groupname2 and other dependent groups will not be added to groupname1; "
    "instead, egg files will reference the textures directly from the "
    "other palettes.\n\n"

    "Comments may appear freely throughout the file, and are set off by a "
    "hash mark (#).\n";
}


////////////////////////////////////////////////////////////////////
//     Function: EggPalettize::run
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void EggPalettize::
run() {
  if (!_txa_filename.exists()) {
    nout << _txa_filename << " does not exist; cannot run.\n";
    exit(1);
  }

  Filename pi_filename = _txa_filename;
  pi_filename.set_extension("boo");

  BamFile pi_file;

  if (!pi_filename.exists()) {
    nout << pi_filename << " does not exist; starting palettization from scratch.\n";
    pal = new Palettizer;

  } else {
    // Read the Palettizer object from the Bam file written
    // previously.  This will recover all of the state saved from the
    // past session.
    if (!pi_file.open_read(pi_filename)) {
      nout << pi_filename << " exists, but cannot be read.  Perhaps you should remove it so a new one can be created.\n";
      exit(1);
    }

    TypedWriteable *obj = pi_file.read_object();
    if (obj == (TypedWriteable *)NULL || !pi_file.resolve()) {
      nout << pi_filename << " exists, but appears to be corrupt.  Perhaps you should remove it so a new one can be created.\n";
      exit(1);
    }

    if (!obj->is_of_type(Palettizer::get_class_type())) {
      nout << pi_filename << " exists, but does not appear to be "
	   << "an egg-palettize output file.  Perhaps you "
	   << "should remove it so a new one can be created.\n";
      exit(1);
    }

    pi_file.close();

    pal = DCAST(Palettizer, obj);
  }

  if (_report_pi) {
    pal->report_pi();
    exit(0);
  }

  if (_got_default_groupname) {
    pal->_default_groupname = _default_groupname;
  } else {
    pal->_default_groupname = _txa_filename.get_basename_wo_extension();
  }

  TxaFile txa_file;
  if (!txa_file.read(_txa_filename)) {
    exit(1);
  }

  Eggs::const_iterator ei;
  for (ei = _eggs.begin(); ei != _eggs.end(); ++ei) {
    EggData *egg_data = (*ei);
    Filename source_filename = egg_data->get_egg_filename();
    Filename dest_filename = get_output_filename(source_filename);
    string name = source_filename.get_basename();

    EggFile *egg_file = pal->get_egg_file(name);
    egg_file->from_command_line(egg_data, source_filename, dest_filename);

    pal->_command_line_eggs.push_back(egg_file);
  }

  pal->run(txa_file);

  if (!pi_file.open_write(pi_filename) ||
      !pi_file.write_object(pal)) {
    nout << "Unable to write palettization information to " << pi_filename
	 << "\n";
    exit(1);
  }

  pi_file.close();

  write_eggs();
}

int 
main(int argc, char *argv[]) {
  EggPalettize prog;
  prog.parse_command_line(argc, argv);
  prog.run();
  return 0;
}

