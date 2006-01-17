// Filename: gtkStats.cxx
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

#include "pandatoolbase.h"
#include "gtkStats.h"
#include "gtkStatsServer.h"
#include "config_pstats.h"

GtkWidget *main_window;
static GtkStatsServer *server = NULL;

static gboolean
delete_event(GtkWidget *widget,
	     GdkEvent *event, gpointer data) {
  // Returning FALSE to indicate we should destroy the main window
  // when the user selects "close".
  return FALSE;
}

static void
destroy(GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}

static gboolean
timer(gpointer data) {
  server->poll();

  return TRUE;
}

int
main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title(GTK_WINDOW(main_window), "PStats");

  // Connect the delete and destroy events, so the user can exit the
  // application by closing the main window.
  g_signal_connect(G_OBJECT(main_window), "delete_event",
		   G_CALLBACK(delete_event), NULL);
  
  g_signal_connect(G_OBJECT(main_window), "destroy",
		   G_CALLBACK(destroy), NULL);

  ostringstream stream;
  stream << "Listening on port " << pstats_port;
  string str = stream.str();
  GtkWidget *label = gtk_label_new(str.c_str());
  gtk_container_add(GTK_CONTAINER(main_window), label);
  gtk_widget_show(label);

  // Create the server object.
  server = new GtkStatsServer;
  if (!server->listen()) {
    ostringstream stream;
    stream 
      << "Unable to open port " << pstats_port
      << ".  Try specifying a different\n"
      << "port number using pstats-port in your Config file.";
    string str = stream.str();

    GtkWidget *dialog = 
      gtk_message_dialog_new(GTK_WINDOW(main_window),
			     GTK_DIALOG_DESTROY_WITH_PARENT,
			     GTK_MESSAGE_ERROR,
			     GTK_BUTTONS_CLOSE,
			     str.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    exit(1);
  }

  gtk_widget_show(main_window);

  // Set up a timer to poll the pstats every so often.
  g_timeout_add(200, timer, NULL);

  // Now get lost in the message loop.
  gtk_main();

  return (0);
}
