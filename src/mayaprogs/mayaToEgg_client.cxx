// Filename: mayaToEgg.cxx
// Adapted by: cbrunner (09Nov09)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "mayaToEgg_client.h"
#ifdef _WIN32
  #include "pystub.h"
#endif

////////////////////////////////////////////////////////////////////
//     Function: MayaToEgg::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
MayaToEggClient::
MayaToEggClient() :
  SomethingToEgg("Maya", ".mb")
{
  qManager = new QueuedConnectionManager();
  qReader = new QueuedConnectionReader(qManager, 0);
  qWriter = new ConnectionWriter(qManager, 0);
  // We assume the server is local and on port 4242
  server.set_host("localhost", 4242);
}

int main(int argc, char *argv[]) {
  // We don't want pystub on linux, since it gives problems with Maya's python.
#ifdef _WIN32
  // A call to pystub() to force libpystub.so to be linked in.
  pystub();
#endif

  MayaToEggClient prog;
  // Open a connection to the server process
  PT(Connection) con = prog.qManager->open_TCP_client_connection(prog.server,0);
  if (con.is_null()) {
    nout << "port opened fail";
  }

  // Add this connection to the readers list
  prog.qReader->add_connection(con);

  // Get the current working directory and make sure it's a string
  Filename cwd = ExecutionEnvironment::get_cwd();
  string s_cwd = (string)cwd.to_os_specific();
  nout << "CWD: " << s_cwd << "\n";

  nout << "building datagram\n";
  NetDatagram datagram;
  
  // First part of the datagram is the argc
  datagram.add_uint8(argc);

  // Add the rest of the arguments as strings to the datagram
  int i;
  for (i = 0; i < argc; i++) {
    datagram.add_string(argv[i]);
  }

  // Lastly, add the current working dir as a string to the datagram
  datagram.add_string(s_cwd);
  nout << "sending datagram\n";
  
  // Send it and close the connection
  prog.qWriter->send(datagram, con);
  prog.qManager->close_connection(con);
  return 0;
}

