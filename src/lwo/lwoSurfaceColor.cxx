// Filename: lwoSurfaceColor.cxx
// Created by:  drose (24Apr01)
// 
////////////////////////////////////////////////////////////////////

#include "lwoSurfaceColor.h"
#include "lwoInputFile.h"

#include <indent.h>

TypeHandle LwoSurfaceColor::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: LwoSurfaceColor::read_iff
//       Access: Public, Virtual
//  Description: Reads the data of the chunk in from the given input
//               file, if possible.  The ID and length of the chunk
//               have already been read.  stop_at is the byte position
//               of the file to stop at (based on the current position
//               at in->get_bytes_read()).  Returns true on success,
//               false otherwise.
////////////////////////////////////////////////////////////////////
bool LwoSurfaceColor::
read_iff(IffInputFile *in, size_t stop_at) {
  LwoInputFile *lin = DCAST(LwoInputFile, in);

  _color = lin->get_vec3();
  _envelope = lin->get_vx();

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: LwoSurfaceColor::write
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
void LwoSurfaceColor::
write(ostream &out, int indent_level) const {
  indent(out, indent_level)
    << get_id() << " { color = " << _color 
     << ", envelope = " << _envelope << " }\n";
}