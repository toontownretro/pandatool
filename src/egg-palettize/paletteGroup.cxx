// Filename: paletteGroup.cxx
// Created by:  drose (30Nov00)
// 
////////////////////////////////////////////////////////////////////

#include "paletteGroup.h"
#include "palettePage.h"
#include "texturePlacement.h"
#include "textureImage.h"

#include <indent.h>
#include <datagram.h>
#include <datagramIterator.h>
#include <bamReader.h>
#include <bamWriter.h>

TypeHandle PaletteGroup::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
PaletteGroup::
PaletteGroup() {
  _egg_count = 0;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::set_dirname
//       Access: Public
//  Description: Sets the directory name associated with the palette
//               group.  This is an optional feature that can be used
//               to place the maps for the different palette groups
//               into different install directories.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
set_dirname(const string &dirname) {
  _dirname = dirname;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::group_with
//       Access: Public
//  Description: Indicates a dependency of this group on some other
//               group.  This means that the textures assigned to this
//               group may be considered successfully assigned if they
//               are actually placed in the other group.  In practice,
//               this means that the textures associated with the
//               other palette group will always be resident at
//               runtime when textures from this palette group are
//               required.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
group_with(PaletteGroup *other) {
  _dependent.insert(other);
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::get_groups
//       Access: Public
//  Description: Returns the set of groups this group depends on.
////////////////////////////////////////////////////////////////////
const PaletteGroups &PaletteGroup::
get_groups() const {
  return _dependent;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::increment_egg_count
//       Access: Public
//  Description: Increments by one the number of egg files that are
//               known to reference this PaletteGroup.  This is
//               designed to aid the heuristics in texture placing;
//               it's useful to know how many different egg files are
//               sharing a particular PaletteGroup.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
increment_egg_count() {
  _egg_count++;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::get_egg_count
//       Access: Public
//  Description: Returns the number of egg files that share this
//               PaletteGroup.
////////////////////////////////////////////////////////////////////
int PaletteGroup::
get_egg_count() const {
  return _egg_count;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::get_page
//       Access: Public
//  Description: Returns the page associated with the indicated
//               properties.  If no page object has yet been created,
//               creates one.
////////////////////////////////////////////////////////////////////
PalettePage *PaletteGroup::
get_page(const TextureProperties &properties) {
  Pages::iterator pi = _pages.find(properties);
  if (pi != _pages.end()) {
    return (*pi).second;
  }

  PalettePage *page = new PalettePage(this, properties);
  bool inserted = _pages.insert(Pages::value_type(properties, page)).second;
  nassertr(inserted, page);
  return page;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::prepare
//       Access: Public
//  Description: Marks the indicated Texture as ready for placing
//               somewhere within this group, and returns a
//               placeholder TexturePlacement object.  The texture is
//               not placed immediately, but may be placed later when
//               place_all() is called; at this time, the
//               TexturePlacement fields will be filled in as
//               appropriate.
////////////////////////////////////////////////////////////////////
TexturePlacement *PaletteGroup::
prepare(TextureImage *texture) {
  TexturePlacement *placement = new TexturePlacement(texture, this);
  _placements.insert(placement);

  return placement;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::unplace
//       Access: Public
//  Description: Removes the texture from its position on a
//               PaletteImage, if it has been so placed.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
unplace(TexturePlacement *placement) {
  nassertv(placement->get_group() == this);

  Placements::iterator pi;
  pi = _placements.find(placement);
  if (pi != _placements.end()) {
    _placements.erase(pi);

    if (placement->is_placed()) {
      placement->get_page()->unplace(placement);
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::place_all
//       Access: Public
//  Description: Once all the textures have been assigned to this
//               group, try to place them all onto suitable
//               PaletteImages.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
place_all() {
  // First, go through our prepared textures and assign each unplaced
  // one to an appropriate page.
  Placements::iterator pli;
  for (pli = _placements.begin(); pli != _placements.end(); ++pli) {
    TexturePlacement *placement = (*pli);
    if (placement->get_omit_reason() == OR_working) {
      PalettePage *page = get_page(placement->get_properties());
      page->assign(placement);
    }
  }

  // Then, go through the pages and actually do the placing.
  Pages::iterator pai;
  for (pai = _pages.begin(); pai != _pages.end(); ++pai) {
    PalettePage *page = (*pai).second;
    page->place_all();
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::write_image_info
//       Access: Public
//  Description: Writes a list of the PaletteImages associated with
//               this group, and all of their textures, to the
//               indicated output stream.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
write_image_info(ostream &out, int indent_level) const {
  Pages::const_iterator pai;
  for (pai = _pages.begin(); pai != _pages.end(); ++pai) {
    PalettePage *page = (*pai).second;
    page->write_image_info(out, indent_level);
  }

  Placements::iterator pli;
  for (pli = _placements.begin(); pli != _placements.end(); ++pli) {
    TexturePlacement *placement = (*pli);
    if (placement->get_omit_reason() != OR_none) {
      indent(out, indent_level) 
	<< placement->get_texture()->get_name()
	<< " unplaced because ";
      switch (placement->get_omit_reason()) {
      case OR_repeats:
	out << "repeats (" 
	    << floor(placement->get_uv_area() * 10000.0 + 0.5) / 100.0
	    << "%)";
	break;

      case OR_size:
	out << "size (" << placement->get_x_size() << " " 
	    << placement->get_y_size() << ")";
	break;

      default:
	out << placement->get_omit_reason();
      }
      out << "\n";
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::update_images
//       Access: Public
//  Description: Regenerates each PaletteImage on this group that needs
//               it.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
update_images() {
  Pages::iterator pai;
  for (pai = _pages.begin(); pai != _pages.end(); ++pai) {
    PalettePage *page = (*pai).second;
    page->update_images();
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::register_with_read_factory
//       Access: Public, Static
//  Description: Registers the current object as something that can be
//               read from a Bam file.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
register_with_read_factory() {
  BamReader::get_factory()->
    register_factory(get_class_type(), make_PaletteGroup);
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::write_datagram
//       Access: Public, Virtual
//  Description: Fills the indicated datagram up with a binary
//               representation of the current object, in preparation
//               for writing to a Bam file.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
write_datagram(BamWriter *writer, Datagram &datagram) {
  datagram.add_string(get_name());
  datagram.add_string(_dirname);
  _dependent.write_datagram(writer, datagram);

  datagram.add_uint32(_placements.size());
  Placements::const_iterator pli;
  for (pli = _placements.begin(); pli != _placements.end(); ++pli) {
    writer->write_pointer(datagram, (*pli));
  }

  datagram.add_uint32(_pages.size());
  Pages::const_iterator pai;
  for (pai = _pages.begin(); pai != _pages.end(); ++pai) {
    writer->write_pointer(datagram, (*pai).second);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::complete_pointers
//       Access: Public, Virtual
//  Description: Called after the object is otherwise completely read
//               from a Bam file, this function's job is to store the
//               pointers that were retrieved from the Bam file for
//               each pointer object written.  The return value is the
//               number of pointers processed from the list.
////////////////////////////////////////////////////////////////////
int PaletteGroup::
complete_pointers(vector_typedWriteable &plist, BamReader *manager) {
  nassertr((int)plist.size() >= _num_placements + _num_pages, 0);
  int index = 0;

  int i;
  for (i = 0; i < _num_placements; i++) {
    TexturePlacement *placement;
    DCAST_INTO_R(placement, plist[index], index);
    index++;
    bool inserted = _placements.insert(placement).second;
    nassertr(inserted, index);
  }

  // We must store the list of pages in a temporary vector first.  We
  // can't put them directly into the map because the map requires
  // that all the pointers in the page's get_properties() member have
  // been filled in, which may not have happened yet.
  _load_pages.reserve(_num_pages);
  for (i = 0; i < _num_pages; i++) {
    PalettePage *page;
    DCAST_INTO_R(page, plist[index], index);
    index++;
    _load_pages.push_back(page);
  }

  return index;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::finalize
//       Access: Public, Virtual
//  Description: This method is called by the BamReader after all
//               pointers everywhere in the world have been completely
//               read in.  It's a hook at which the object can do
//               whatever final setup it requires that depends on
//               other pointers being valid.
////////////////////////////////////////////////////////////////////
void PaletteGroup::
finalize() {
  // Now we can copy the pages into the actual map.
  vector<PalettePage *>::const_iterator pi;
  for (pi = _load_pages.begin(); pi != _load_pages.end(); ++pi) {
    PalettePage *page = (*pi);
    bool inserted = _pages.
      insert(Pages::value_type(page->get_properties(), page)).second;
    nassertv(inserted);
  }

  _load_pages.clear();
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::make_PaletteGroup
//       Access: Protected, Static
//  Description: This method is called by the BamReader when an object
//               of this type is encountered in a Bam file; it should
//               allocate and return a new object with all the data
//               read.
////////////////////////////////////////////////////////////////////
TypedWriteable *PaletteGroup::
make_PaletteGroup(const FactoryParams &params) {
  PaletteGroup *me = new PaletteGroup;
  BamReader *manager;
  Datagram packet;

  parse_params(params, manager, packet);
  DatagramIterator scan(packet);

  me->fillin(scan, manager);
  manager->register_finalize(me);
  return me;
}

////////////////////////////////////////////////////////////////////
//     Function: PaletteGroup::fillin
//       Access: Protected
//  Description: Reads the binary data from the given datagram
//               iterator, which was written by a previous call to
//               write_datagram().
////////////////////////////////////////////////////////////////////
void PaletteGroup::
fillin(DatagramIterator &scan, BamReader *manager) {
  set_name(scan.get_string());
  _dirname = scan.get_string();
  _dependent.fillin(scan, manager);

  _num_placements = scan.get_uint32();
  manager->read_pointers(scan, this, _num_placements);

  _num_pages = scan.get_uint32();
  manager->read_pointers(scan, this, _num_pages);
}