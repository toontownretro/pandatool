// Filename: xFileArrayDef.cxx
// Created by:  drose (03Oct04)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#include "xFileArrayDef.h"
#include "xFileDataDef.h"

////////////////////////////////////////////////////////////////////
//     Function: XFileArrayDef::output
//       Access: Public
//  Description: 
////////////////////////////////////////////////////////////////////
void XFileArrayDef::
output(ostream &out) const {
  if (is_fixed_size()) {
    out << "[" << _fixed_size << "]";
  } else {
    out << "[" << _dynamic_size->get_name() << "]";
  }
}