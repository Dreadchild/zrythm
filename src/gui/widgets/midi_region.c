/*
 * Copyright (C) 2018-2019 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "audio/bus_track.h"
#include "audio/channel.h"
#include "audio/instrument_track.h"
#include "audio/midi_note.h"
#include "audio/track.h"
#include "gui/widgets/arranger.h"
#include "gui/widgets/center_dock.h"
#include "gui/widgets/main_window.h"
#include "gui/widgets/midi_arranger.h"
#include "gui/widgets/midi_region.h"
#include "gui/widgets/region.h"
#include "gui/widgets/ruler.h"
#include "gui/widgets/timeline_arranger.h"

G_DEFINE_TYPE (MidiRegionWidget,
               midi_region_widget,
               REGION_WIDGET_TYPE)


#define Y_PADDING 6.f
#define Y_HALF_PADDING 3.f

static gboolean
draw_cb (MidiRegionWidget * self, cairo_t *cr, gpointer data)
{
  REGION_WIDGET_GET_PRIVATE (data);
  guint width, height;
  GtkStyleContext *context;

  context =
    gtk_widget_get_style_context (GTK_WIDGET (self));

  width =
    gtk_widget_get_allocated_width (GTK_WIDGET (self));
  height =
    gtk_widget_get_allocated_height (GTK_WIDGET (self));

  gtk_render_background (context, cr, 0, 0, width, height);

  GdkRGBA * color = &rw_prv->region->track->color;

  cairo_set_source_rgba (cr,
                         color->red - 0.3,
                         color->green - 0.3,
                         color->blue - 0.3,
                         0.9);

  MidiRegion * mr =
    (MidiRegion *) rw_prv->region;
  Region * r = (Region *) mr;
  int num_loops =
    region_get_num_loops (r, 1);
  long ticks_in_region =
    region_get_full_length_in_ticks (
      (Region *) mr);
  float x_start, y_start, x_end;

  int min_val = 127, max_val = 0;
  for (int i = 0; i < mr->num_midi_notes; i++)
    {
      MidiNote * mn = mr->midi_notes[i];

      if (mn->val < min_val)
        min_val = mn->val;
      if (mn->val > max_val)
        max_val = mn->val;
    }
  float y_interval = MAX ((max_val - min_val) + 1.f,
                          12.f);
  float y_note_size = 1.f / y_interval;

  /* draw midi notes */
  long loop_end_ticks =
    position_to_ticks (&r->loop_end_pos);
  long loop_ticks =
    region_get_loop_length_in_ticks (r);
  int px =
    ui_pos_to_px_timeline (&r->loop_start_pos, 0);
  Position tmp;
  for (int i = 0; i < mr->num_midi_notes; i++)
    {
      MidiNote * mn = mr->midi_notes[i];

      /* get ratio (0.0 - 1.0) on x where midi note starts
       * & ends */
      int mn_start_ticks =
        position_to_ticks (&mn->start_pos);
      int mn_end_ticks =
        position_to_ticks (&mn->end_pos);
      int tmp_start_ticks, tmp_end_ticks;
      /*x_start =*/
        /*(float) mn_start_ticks /*/
        /*ticks_in_region;*/
      /*x_end =*/
        /*(float) mn_end_ticks /*/
        /*ticks_in_region;*/

      /* if note starts before the loop ends and
       * continues after the loop (should be
       * clipped) */
      if (position_compare (
            &mn->start_pos, &r->loop_end_pos) < 0 &&
          position_compare (
            &mn->end_pos, &r->loop_end_pos) >= 0)
        {
          for (int j = 0;
               j < num_loops;
               j++)
            {
              /* calculate draw endpoints */
              tmp_start_ticks =
                mn_start_ticks + loop_ticks * j;
              tmp_end_ticks =
                loop_end_ticks + loop_ticks * j;
              x_start =
                (float) tmp_start_ticks /
                ticks_in_region;
              x_end =
                (float) tmp_end_ticks /
                ticks_in_region;

              /* get ratio (0.0 - 1.0) on y where
               * midi note is */
              y_start =
                (max_val - mn->val) / y_interval;

              /* draw */
              cairo_rectangle (
                cr,
                x_start * width,
                y_start * height,
                (x_end - x_start) * width,
                y_note_size * height);
              cairo_fill (cr);
            }
        }

      /* get ratio (0.0 - 1.0) on y where midi note is */
      /*y_start =*/
        /*(max_val - mn->val) / y_interval;*/

      /*[> draw <]*/
      /*cairo_rectangle (cr,*/
                       /*x_start * width,*/
                       /*y_start * height,*/
                       /*(x_end - x_start) * width,*/
                       /*y_note_size * height);*/
      /*cairo_fill (cr);*/
    }


 return FALSE;
}

/**
 * Sets the appropriate cursor.
 */
static int
on_motion (GtkWidget * widget,
           GdkEventMotion *event,
           MidiRegionWidget * self)
{
  GtkAllocation allocation;
  gtk_widget_get_allocation (widget,
                             &allocation);
  ArrangerWidgetPrivate * prv =
    arranger_widget_get_private (
      Z_ARRANGER_WIDGET (MW_TIMELINE));
  REGION_WIDGET_GET_PRIVATE (self);

  if (event->type == GDK_MOTION_NOTIFY)
    {
      if (event->x < RESIZE_CURSOR_SPACE)
        {
          rw_prv->cursor_state =
            UI_CURSOR_STATE_RESIZE_L;
          if (prv->action !=
                UI_OVERLAY_ACTION_MOVING)
            ui_set_cursor (widget, "w-resize");
        }
      else if (event->x > allocation.width -
                 RESIZE_CURSOR_SPACE)
        {
          rw_prv->cursor_state =
            UI_CURSOR_STATE_RESIZE_R;
          if (prv->action !=
                UI_OVERLAY_ACTION_MOVING)
            ui_set_cursor (widget, "e-resize");
        }
      else
        {
          rw_prv->cursor_state =
            UI_CURSOR_STATE_DEFAULT;
          if (prv->action !=
                UI_OVERLAY_ACTION_MOVING &&
              prv->action !=
                UI_OVERLAY_ACTION_STARTING_MOVING &&
              prv->action !=
                UI_OVERLAY_ACTION_RESIZING_L &&
              prv->action !=
                UI_OVERLAY_ACTION_RESIZING_R)
            {
              ui_set_cursor (widget, "default");
            }
        }
    }
  /* if leaving */
  else if (event->type == GDK_LEAVE_NOTIFY)
    {
      if (prv->action !=
            UI_OVERLAY_ACTION_MOVING &&
          prv->action !=
            UI_OVERLAY_ACTION_RESIZING_L &&
          prv->action !=
            UI_OVERLAY_ACTION_RESIZING_R)
        {
          ui_set_cursor (widget, "default");
        }
    }
  return FALSE;
}

MidiRegionWidget *
midi_region_widget_new (MidiRegion * midi_region)
{
  MidiRegionWidget * self =
    g_object_new (MIDI_REGION_WIDGET_TYPE,
                  NULL);

  region_widget_setup (Z_REGION_WIDGET (self),
                       (Region *) midi_region);
  REGION_WIDGET_GET_PRIVATE (self);

  /* connect signals */
  g_signal_connect (
    G_OBJECT (rw_prv->drawing_area), "draw",
    G_CALLBACK (draw_cb), self);
  g_signal_connect (
    G_OBJECT (rw_prv->drawing_area),
    "enter-notify-event",
    G_CALLBACK (on_motion),  self);
  g_signal_connect (
    G_OBJECT(rw_prv->drawing_area),
    "leave-notify-event",
    G_CALLBACK (on_motion),  self);
  g_signal_connect (
    G_OBJECT(rw_prv->drawing_area),
    "motion-notify-event",
    G_CALLBACK (on_motion),  self);

  return self;
}

static void
midi_region_widget_class_init (MidiRegionWidgetClass * klass)
{
}

static void
midi_region_widget_init (MidiRegionWidget * self)
{
}
