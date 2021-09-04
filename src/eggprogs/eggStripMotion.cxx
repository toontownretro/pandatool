/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file eggStripMotion.cxx
 * @author brian
 * @date 2021-04-28
 */

#include "eggStripMotion.h"
#include "eggTable.h"
#include "eggXfmAnimData.h"
#include "eggXfmSAnim.h"
#include "dcast.h"

/**
 *
 */
EggStripMotion::
EggStripMotion() {
  set_program_brief("strip part or all animation from a joint");
  set_program_description
    ("egg-strip-motion can strip part or all animation from a particular joint "
     "of an animation.  This is particularly useful for movement animations, "
     "where it is generally easier to animate when the movement of the character "
     "is part of the animation, but the animation needs to be stationary for use "
     "in-game.");

  add_option
    ("joint", "name", 0,
     "Specify the name of the joint to strip animation from.",
     &EggStripMotion::dispatch_string, nullptr, &_joint);

  add_option
    ("components", "xyzhprijkabc", 0,
     "Specify the transform components to strip from the animation of the joint.",
     &EggStripMotion::dispatch_string, nullptr, &_components);
}

EggNode *
r_find_child(EggGroupNode *parent, const std::string &name) {
  EggNode *child = parent->find_child(name);
  if (child != nullptr) {
    return child;
  }

  for (EggGroupNode::iterator it = parent->begin(); it != parent->end(); ++it) {
    EggNode *other = *it;
    if (!other->is_of_type(EggGroupNode::get_class_type())) {
      continue;
    }
    child = r_find_child((EggGroupNode *)other, name);
    if (child != nullptr) {
      return child;
    }
  }

  return nullptr;
}

/**
 *
 */
bool EggStripMotion::
run() {
  if (_joint.empty()) {
    nout << "No joint specified!\n";
    return false;
  }

  if (_components.empty()) {
    nout << "No components specified!\n";
    return false;
  }

  EggNode *node = r_find_child(_data, _joint);
  if (node == nullptr) {
    nout << "Joint " << _joint << " not found in animation.\n";
    return false;
  }

  std::cout << "Found joint\n";

  if (!node->is_of_type(EggTable::get_class_type()) ||
      DCAST(EggTable, node)->get_table_type() != EggTable::TT_table) {
    nout << _joint << " is not a <Table>\n";
    return false;
  }

  EggTable *table = DCAST(EggTable, node);

  pvector<PT(EggNode)> converted;

  for (EggTable::iterator it = table->begin(); it != table->end();) {
    EggNode *node = (*it);
    if (!node->is_of_type(EggXfmSAnim::get_class_type()) &&
        !node->is_of_type(EggXfmAnimData::get_class_type())) {
      ++it;
      continue;
    }

    std::cout << "Doign stuff\n";
    PT(EggXfmSAnim) xfm;
    if (node->is_of_type(EggXfmSAnim::get_class_type())) {
      xfm = DCAST(EggXfmSAnim, node);
      ++it;
    } else {
      xfm = new EggXfmSAnim(*DCAST(EggXfmAnimData, node));
      it = table->erase(it);
      converted.push_back(xfm);
    }

    std::cout << "stripping\n";

    for (int i = 0; i < xfm->get_num_rows(); i++) {
      xfm->normalize();

      LMatrix4d value;
      xfm->get_value(i, value);

      LPoint3d pos;
      LVecBase3d hpr;
      LVecBase3d scale;
      LVecBase3d shear;
      decompose_matrix(value, scale, shear, hpr, pos);

      for (char c : _components) {
        switch (c) {
        case 'x':
          pos[0] = 0.0;
          break;
        case 'y':
          pos[1] = 0.0;
          break;
        case 'z':
          pos[2] = 0.0;
          break;
        case 'h':
          hpr[0] = 0.0;
          break;
        case 'p':
          hpr[1] = 0.0;
          break;
        case 'r':
          hpr[2] = 0.0;
          break;
        case 'i':
          scale[0] = 1.0;
          break;
        case 'j':
          scale[1] = 1.0;
          break;
        case 'k':
          scale[2] = 1.0;
          break;
        case 'a':
          shear[0] = 0.0;
          break;
        case 'b':
          shear[1] = 0.0;
          break;
        case 'c':
          shear[2] = 0.0;
          break;
        default:
          break;
        }
      }

      compose_matrix(value, scale, shear, hpr, pos);

      xfm->set_value(i, value);
      xfm->optimize_to_standard_order();
    }
  }

  for (auto it = converted.begin(); it != converted.end(); ++it) {
    table->add_child(*it);
  }

  write_egg_file();

  return true;
}

int
main(int argc, char *argv[]) {
  EggStripMotion prog;
  prog.parse_command_line(argc, argv);
  return prog.run() ? 0 : 1;
}
