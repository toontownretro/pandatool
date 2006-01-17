// Filename: gtkStatsLabelStack.cxx
// Created by:  drose (16Jan06)
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

#include "gtkStatsLabelStack.h"
#include "gtkStatsLabel.h"
#include "notify.h"

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
GtkStatsLabelStack::
GtkStatsLabelStack() {
  _widget = NULL;

  _highlight_label = -1;
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::Destructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
GtkStatsLabelStack::
~GtkStatsLabelStack() {
  clear_labels();
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::setup
//       Access: Public
//  Description: Creates the actual widget object.
////////////////////////////////////////////////////////////////////
GtkWidget *GtkStatsLabelStack::
setup() {
  if (_widget == NULL) {
    _widget = gtk_vbox_new(FALSE, 0);
  }
 
  return _widget;
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::is_setup
//       Access: Public
//  Description: Returns true if the label stack has been set up,
//               false otherwise.
////////////////////////////////////////////////////////////////////
bool GtkStatsLabelStack::
is_setup() const {
  return (_widget != NULL);
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::get_label_collector_index
//       Access: Public
//  Description: Returns the collector index associated with the
//               indicated label.
////////////////////////////////////////////////////////////////////
int GtkStatsLabelStack::
get_label_collector_index(int label_index) const {
  nassertr(label_index >= 0 && label_index < (int)_labels.size(), -1);
  return _labels[label_index]->get_collector_index();
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::clear_labels
//       Access: Public
//  Description: Removes the set of labels and starts a new set.
////////////////////////////////////////////////////////////////////
void GtkStatsLabelStack::
clear_labels() {
  Labels::iterator li;
  for (li = _labels.begin(); li != _labels.end(); ++li) {
    GtkStatsLabel *label = (*li);
    gtk_container_remove(GTK_CONTAINER(_widget), label->get_widget());
    delete (*li);
  }
  _labels.clear();
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::add_label
//       Access: Public
//  Description: Adds a new label to the top of the stack; returns the
//               new label index.
////////////////////////////////////////////////////////////////////
int GtkStatsLabelStack::
add_label(GtkStatsMonitor *monitor, GtkStatsGraph *graph,
          int thread_index, int collector_index, bool use_fullname) {
  GtkStatsLabel *label = 
    new GtkStatsLabel(monitor, graph, thread_index, collector_index, use_fullname);

  gtk_box_pack_start(GTK_BOX(_widget), label->setup(),
		     FALSE, FALSE, 0);

  int label_index = (int)_labels.size();
  _labels.push_back(label);

  return label_index;
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::get_num_labels
//       Access: Public
//  Description: Returns the number of labels in the stack.
////////////////////////////////////////////////////////////////////
int GtkStatsLabelStack::
get_num_labels() const {
  return _labels.size();
}

////////////////////////////////////////////////////////////////////
//     Function: GtkStatsLabelStack::highlight_label
//       Access: Public
//  Description: Draws a highlight around the label representing the
//               indicated collector, and removes the highlight from
//               any other label.  Specify -1 to remove the highlight
//               from all labels.
////////////////////////////////////////////////////////////////////
void GtkStatsLabelStack::
highlight_label(int collector_index) {
  if (_highlight_label != collector_index) {
    _highlight_label = collector_index;
    Labels::iterator li;
    for (li = _labels.begin(); li != _labels.end(); ++li) {
      GtkStatsLabel *label = (*li);
      label->set_highlight(label->get_collector_index() == _highlight_label);
    }
  }
}
