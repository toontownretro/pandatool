// Filename: xFileDataObjectTemplate.cxx
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

#include "xFileDataObjectTemplate.h"
#include "indent.h"
#include "xFileParseData.h"
#include "xLexerDefs.h"

TypeHandle XFileDataObjectTemplate::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
XFileDataObjectTemplate::
XFileDataObjectTemplate(XFile *x_file, const string &name,
                        XFileTemplate *xtemplate) :
  XFileNode(x_file, name),
  _template(xtemplate)
{
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::write_text
//       Access: Public, Virtual
//  Description: Writes a suitable representation of this node to an
//               .x file in text mode.
////////////////////////////////////////////////////////////////////
void XFileDataObjectTemplate::
write_text(ostream &out, int indent_level) const {
  indent(out, indent_level)
    << _template->get_name();
  if (has_name()) {
    out << " " << get_name();
  }
  out << " {\n";

  int num_elements = get_num_elements();
  for (int i = 0; i < num_elements; i++) {
    get_element(i)->write_data(out, indent_level + 2, ";");
  }

  XFileNode::write_text(out, indent_level + 2);
  indent(out, indent_level)
    << "}\n";
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::write_data
//       Access: Public, Virtual
//  Description: Writes a suitable representation of this node to an
//               .x file in text mode.
////////////////////////////////////////////////////////////////////
void XFileDataObjectTemplate::
write_data(ostream &out, int indent_level, const char *separator) const {
  if (!_nested_elements.empty()) {
    if (_nested_elements.front()->size() != 0) {
      // If we have a complex nested structure, output one per line.
      for (size_t i = 0; i < _nested_elements.size() - 1; i++) {
        _nested_elements[i]->write_data(out, indent_level, ";");
      }
      string combined_separator = string(";") + string(separator);
      _nested_elements.back()->write_data(out, indent_level, 
                                          combined_separator.c_str());

    } else {
      // Otherwise, output them all on the same line.
      indent(out, indent_level);
      for (size_t i = 0; i < _nested_elements.size() - 1; i++) {
        out << *_nested_elements[i] << "; ";
      }
      out << *_nested_elements.back() << ";" << separator << "\n";
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::add_parse_object
//       Access: Public
//  Description: Adds the indicated object as a nested object
//               encountered in the parser.  It will later be
//               processed by finalize_parse_data().
////////////////////////////////////////////////////////////////////
void XFileDataObjectTemplate::
add_parse_object(XFileDataObjectTemplate *object, bool reference) {
  XFileParseData pdata;
  pdata._object = object;
  pdata._parse_flags = XFileParseData::PF_object;
  if (reference) {
    pdata._parse_flags |= XFileParseData::PF_reference;
  }

  _parse_data_list._list.push_back(pdata);
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::add_parse_double
//       Access: Public
//  Description: Adds the indicated list of doubles as a data element
//               encountered in the parser.  It will later be
//               processed by finalize_parse_data().
////////////////////////////////////////////////////////////////////
void XFileDataObjectTemplate::
add_parse_double(PTA_double double_list, char separator) {
  XFileParseData pdata;
  pdata._double_list = double_list;
  pdata._parse_flags = XFileParseData::PF_double;
  switch (separator) {
  case ',':
    pdata._parse_flags |= XFileParseData::PF_comma;
    break;

  case ';':
    pdata._parse_flags |= XFileParseData::PF_semicolon;
    break;
  }

  _parse_data_list._list.push_back(pdata);
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::add_parse_int
//       Access: Public
//  Description: Adds the indicated list of ints as a data element
//               encountered in the parser.  It will later be
//               processed by finalize_parse_data().
////////////////////////////////////////////////////////////////////
void XFileDataObjectTemplate::
add_parse_int(PTA_int int_list, char separator) {
  XFileParseData pdata;
  pdata._int_list = int_list;
  pdata._parse_flags = XFileParseData::PF_int;
  switch (separator) {
  case ',':
    pdata._parse_flags |= XFileParseData::PF_comma;
    break;

  case ';':
    pdata._parse_flags |= XFileParseData::PF_semicolon;
    break;
  }

  _parse_data_list._list.push_back(pdata);
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::add_parse_string
//       Access: Public
//  Description: Adds the indicated string as a data element
//               encountered in the parser.  It will later be
//               processed by finalize_parse_data().
////////////////////////////////////////////////////////////////////
void XFileDataObjectTemplate::
add_parse_string(const string &str, char separator) {
  XFileParseData pdata;
  pdata._string = str;
  pdata._parse_flags = XFileParseData::PF_string;
  switch (separator) {
  case ',':
    pdata._parse_flags |= XFileParseData::PF_comma;
    break;

  case ';':
    pdata._parse_flags |= XFileParseData::PF_semicolon;
    break;
  }

  _parse_data_list._list.push_back(pdata);
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::add_parse_separator
//       Access: Public
//  Description: Adds the indicated separator character as an isolated
//               separator encountered in the parser.  It will later
//               be processed by finalize_parse_data().
////////////////////////////////////////////////////////////////////
void XFileDataObjectTemplate::
add_parse_separator(char separator) {
  XFileParseData pdata;
  pdata._parse_flags = 0;
  switch (separator) {
  case ',':
    pdata._parse_flags |= XFileParseData::PF_comma;
    break;

  case ';':
    pdata._parse_flags |= XFileParseData::PF_semicolon;
    break;
  }

  _parse_data_list._list.push_back(pdata);
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::finalize_parse_data
//       Access: Public
//  Description: Processes all of the data elements added by
//               add_parse_*(), checks them for syntactic and semantic
//               correctness against the Template definition, and
//               stores the appropriate child data elements.  Returns
//               true on success, false if there is a mismatch.
////////////////////////////////////////////////////////////////////
bool XFileDataObjectTemplate::
finalize_parse_data() {
  // Recursively walk through our template definition, while
  // simultaneously walking through the list of parse data elements we
  // encountered, and re-pack them as actual nested elements.
  PrevData prev_data;
  size_t index = 0;
  size_t sub_index = 0;

  if (!_template->repack_data(this, _parse_data_list, 
                              prev_data, index, sub_index)) {
    return false;
  }

  // Quietly allow an extra semicolon at the end of the structure.
  // (Why is this sometimes here?)
  if (index < _parse_data_list._list.size() &&
      _parse_data_list._list[index]._parse_flags == XFileParseData::PF_semicolon) {
    index++;
  }

  if (index != _parse_data_list._list.size()) {
    cerr << "flags = " << hex << _parse_data_list._list[index]._parse_flags << dec << "\n";
    xyyerror("Too many data elements in structure.");
    return false;
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::add_element
//       Access: Public, Virtual
//  Description: Adds the indicated element as a nested data element,
//               if this data object type supports it.  Returns true
//               if added successfully, false if the data object type
//               does not support nested data elements.
////////////////////////////////////////////////////////////////////
bool XFileDataObjectTemplate::
add_element(XFileDataObject *element) {
  _nested_elements.push_back(element);
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::get_num_elements
//       Access: Protected, Virtual
//  Description: Returns the number of nested data elements within the
//               object.  This may be, e.g. the size of the array, if
//               it is an array.
////////////////////////////////////////////////////////////////////
int XFileDataObjectTemplate::
get_num_elements() const {
  return _nested_elements.size();
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::get_element
//       Access: Protected, Virtual
//  Description: Returns the nth nested data element within the
//               object.
////////////////////////////////////////////////////////////////////
const XFileDataObject *XFileDataObjectTemplate::
get_element(int n) const {
  nassertr(n >= 0 && n < (int)_nested_elements.size(), NULL);
  return _nested_elements[n];
}

////////////////////////////////////////////////////////////////////
//     Function: XFileDataObjectTemplate::get_element
//       Access: Protected, Virtual
//  Description: Returns the nested data element within the
//               object that has the indicated name.
////////////////////////////////////////////////////////////////////
const XFileDataObject *XFileDataObjectTemplate::
get_element(const string &name) const {
  return NULL;
}
