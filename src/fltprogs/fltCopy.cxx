// Filename: fltCopy.cxx
// Created by:  drose (01Nov00)
// 
////////////////////////////////////////////////////////////////////

#include "fltCopy.h"

#include <cvsSourceDirectory.h>
#include <fltHeader.h>
#include <fltFace.h>
#include <fltExternalReference.h>
#include <fltError.h>

////////////////////////////////////////////////////////////////////
//     Function: FltCopy::Constructor
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
FltCopy::
FltCopy() {
  set_program_description
    ("fltcopy copies one or more MultiGen .flt files into a "
     "CVS source hierarchy.  "
     "Rather than copying the named files immediately into the current "
     "directory, it first scans the entire source hierarchy, identifying all "
     "the already-existing files.  If the named file to copy matches the "
     "name of an already-existing file in the current directory or elsewhere "
     "in the hierarchy, that file is overwritten.  Other .flt files, as "
     "well as texture files, that are externally referenced by the "
     "named .flt file(s) are similarly copied.");

  clear_runlines();
  add_runline("[opts] file.flt [file.flt ... ]");

  add_option
    ("s", "dirname", 0, 
     "Specify the directory or directories that are to be searched for "
     "relative pathnames appearing in the flt file.  This may be a "
     "single directory name or a colon-delimited list of directories.  "
     "It may also be " 
     "repeated multiple times on the command line; each time it appears "
     "its named directories will be appended to the search path.",
     &FltCopy::dispatch_search_path, NULL, &_search_path);
}

////////////////////////////////////////////////////////////////////
//     Function: FltCopy::run
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
void FltCopy::
run() {
  SourceFiles::iterator fi;
  for (fi = _source_files.begin(); fi != _source_files.end(); ++fi) {
    ExtraData ed;
    ed._type = FT_flt;

    CVSSourceDirectory *dest = import(*fi, &ed, _model_dir);
    if (dest == (CVSSourceDirectory *)NULL) {
      exit(1);
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: FltCopy::copy_file
//       Access: Protected, Virtual
//  Description: Called by import() if verify_file() indicates that a
//               file needs to be copied.  This does the actual copy
//               of a file from source to destination.  If new_file is
//               true, then dest does not already exist.
////////////////////////////////////////////////////////////////////
bool FltCopy::
copy_file(const Filename &source, const Filename &dest,
	  CVSSourceDirectory *dir, void *extra_data, bool new_file) {
  ExtraData *ed = (ExtraData *)extra_data;
  switch (ed->_type) {
  case FT_flt:
    return copy_flt_file(source, dest, dir);

  case FT_texture:
    return copy_texture(source, dest, dir, ed->_texture, new_file);
  }

  nout << "Internal error: invalid type " << (int)ed->_type << "\n";
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: FltCopy::copy_flt_file
//       Access: Private
//  Description: 
////////////////////////////////////////////////////////////////////
bool FltCopy::
copy_flt_file(const Filename &source, const Filename &dest,
	      CVSSourceDirectory *dir) {
  PT(FltHeader) header = new FltHeader;
  header->set_texture_path(_search_path);
  header->set_model_path(_search_path);

  // We don't want to automatically generate .attr files--we'd rather
  // write them out explicitly.
  header->set_auto_attr_update(FltHeader::AU_none);

  FltError result = header->read_flt(source);
  if (result != FE_ok) {
    nout << "Cannot read " << source << ": " << result << "\n";
    return false;
  }

  // Now scan the flt file for nested references.
  Refs refs;
  Textures textures;
  scan_flt(header, refs, textures);

  Refs::const_iterator ri;
  for (ri = refs.begin(); ri != refs.end(); ++ri) {
    FltExternalReference *ref = (*ri);
    Filename ref_filename = ref->get_ref_filename();

    if (!ref_filename.exists()) {
      nout << "*** Warning: external reference " << ref_filename
	   << " does not exist.\n";
    } else {
      ExtraData ed;
      ed._type = FT_flt;
      
      CVSSourceDirectory *ref_dir =
	import(ref_filename, &ed, _model_dir);
      if (ref_dir == (CVSSourceDirectory *)NULL) {
	return false;
      }
      
      // Update the reference to point to the new flt filename, relative
      // to the base flt file.
      ref->_filename = dir->get_rel_to(ref_dir) + "/" + 
	ref_filename.get_basename();
    }
  }

  // Remove all the textures from the palette, and then add back only
  // those we found in use.  This way we don't copy a file that
  // references bogus textures.
  header->clear_textures();

  Textures::const_iterator ti;
  for (ti = textures.begin(); ti != textures.end(); ++ti) {
    FltTexture *tex = (*ti);
    Filename texture_filename = tex->get_texture_filename();

    if (!texture_filename.exists()) {
      nout << "*** Warning: texture " << texture_filename
	   << " does not exist.\n";
    } else {
      ExtraData ed;
      ed._type = FT_texture;
      ed._texture = tex;
      
      CVSSourceDirectory *texture_dir =
	import(texture_filename, &ed, _map_dir);
      if (texture_dir == (CVSSourceDirectory *)NULL) {
	return false;
      }
      
      // Update the texture reference to point to the new texture
      // filename, relative to the flt file.
      tex->_filename = dir->get_rel_to(texture_dir) + "/" + 
	texture_filename.get_basename();
      header->add_texture(tex);
    }
  }

  // Finally, write the resulting file out.
  result = header->write_flt(dest);
  if (result != FE_ok) {
    nout << "Cannot write " << dest << "\n";
    return false;
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: FltCopy::copy_texture
//       Access: Private
//  Description: 
////////////////////////////////////////////////////////////////////
bool FltCopy::
copy_texture(const Filename &source, const Filename &dest, 
	     CVSSourceDirectory *dir, FltTexture *tex, bool new_file) {
  if (!copy_binary_file(source, dest)) {
    return false;
  }

  // Also write out the .attr file.
  Filename attr_filename = dest.get_fullpath() + ".attr";
  if (!attr_filename.exists()) {
    new_file = true;
  }

  tex->write_attr_data(attr_filename);

  if (new_file) {
    cvs_add(attr_filename);
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: FltCopy::scan_flt
//       Access: Private
//  Description: Recursively walks through the flt file hierarchy,
//               looking for texture references and external flt file
//               references.
////////////////////////////////////////////////////////////////////
void FltCopy::
scan_flt(FltRecord *record, FltCopy::Refs &refs, FltCopy::Textures &textures) {
  if (record->is_of_type(FltFace::get_class_type())) {
    FltFace *face;
    DCAST_INTO_V(face, record);
    if (face->has_texture()) {
      textures.insert(face->get_texture());
    }

  } else if (record->is_of_type(FltExternalReference::get_class_type())) {
    FltExternalReference *ref;
    DCAST_INTO_V(ref, record);

    refs.insert(ref);
  }

  int i;
  int num_subfaces = record->get_num_subfaces();
  for (i = 0; i < num_subfaces; i++) {
    scan_flt(record->get_subface(i), refs, textures);
  }

  int num_children = record->get_num_children();
  for (i = 0; i < num_children; i++) {
    scan_flt(record->get_child(i), refs, textures);
  }
}


int main(int argc, char *argv[]) {
  FltCopy prog;
  prog.parse_command_line(argc, argv);
  prog.run();
  return 0;
}