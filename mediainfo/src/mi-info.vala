/* GStreamer media browser
 * Copyright (C) 2010 Stefan Sauer <ensonic@user.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Steet,
 * Boston, MA 02110-1301, USA.
 */

using Gtk;
using Gst;

public class MediaInfo.Info : VBox
{
  private Label uri;
  //private Discoverer dc;
  private DrawingArea drawing_area;
  private Pipeline pb;

  public Info ()
  {
    // configure the view
    set_homogeneous (false);

    // add widgets
    drawing_area = new DrawingArea ();
    drawing_area.set_size_request (300, 150);
    pack_start (drawing_area, true, true, 0);

    uri = new Label ("");
    pack_start (uri, false, false, 0);

    show_all ();

    // set up the gstreamer components
    //dc = new Discoverer (Gst.SECONDS * 10, null);

    pb = ElementFactory.make ("playbin2", "player") as Pipeline;
    // FIXME: need bus callback for:
    // xoverlay.set_xwindow_id (Gdk.x11_drawable_get_xid (this.drawing_area.window));
    Gst.Bus bus = pb.get_bus ();
    bus.add_signal_watch ();
    bus.message["element"].connect (on_element_message);

  }

  // public methods

  public bool discover (string uri)
  {
    // TODO: stop previous playback (also need destoructor)
    
    this.uri.set_text (uri);
    
    //DiscovererInfo info = dc.discover_uri (uri, null);

    // TODO: play file
    //pb.uri = uri;
    //pb.set_state (State.PLAYING);

    
    return (true);
  }

  // signal handlers

  private void on_element_message (Gst.Bus bus, Message message)
  {
    Structure structure = message.get_structure ();
    if (structure.has_name ("prepare-xwindow-id"))
    {
      //XOverlay xoverlay = message.src as XOverlay;
      //xoverlay.set_xwindow_id (Gdk.x11_drawable_get_xid (this.drawing_area.window));
    }
  }
}