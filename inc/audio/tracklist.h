/*
 * Copyright (C) 2018-2019 Alexandros Theodotou
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

/**
 * \file
 *
 * Tracklist backend.
 */

#ifndef __AUDIO_TRACKLIST_H__
#define __AUDIO_TRACKLIST_H__

#include "audio/engine.h"

/**
 * @addtogroup audio
 *
 * @{
 */

typedef struct Track Track;
typedef struct _TracklistWidget TracklistWidget;

#define TRACKLIST (&PROJECT->tracklist)
#define MAX_TRACKS 3000

typedef struct Track ChordTrack;

/**
 * The Tracklist contains all the tracks in the
 * Project.
 *
 * There should be a clear separation between the
 * Tracklist and the Mixer. The Tracklist should be
 * concerned with Tracks in the arranger, and the
 * Mixer should be concerned with Channels, routing
 * and Port connections.
 */
typedef struct Tracklist
{
  /**
   * All tracks that exist.
   *
   * These should always be sorted in the same way
   * they should appear in the GUI.
   */
  Track *             tracks[MAX_TRACKS];

  /**
   * Track IDs corresponding to the above \p tracks.
   */
  int                 track_ids[MAX_TRACKS];
  int                 num_tracks;

  TracklistWidget *   widget;
} Tracklist;

static const cyaml_schema_field_t
  tracklist_fields_schema[] =
{
  CYAML_FIELD_SEQUENCE_COUNT (
    /* default because it is an array of pointers, not a
     * pointer to an array */
    "track_ids", CYAML_FLAG_DEFAULT,
      Tracklist, track_ids, num_tracks,
      &int_schema, 0, CYAML_UNLIMITED),

	CYAML_FIELD_END
};

static const cyaml_schema_value_t
tracklist_schema = {
	CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER,
			Tracklist, tracklist_fields_schema),
};

/**
 * Initializes the tracklist when loading a project.
 */
void
tracklist_init_loaded (
  Tracklist * self);

/**
 * Finds visible tracks and puts them in given array.
 */
void
tracklist_get_visible_tracks (
  Track **    visible_tracks,
  int *       num_visible);

int
tracklist_contains_master_track ();

int
tracklist_contains_chord_track ();

/**
 * Adds given track to given spot in tracklist.
 */
void
tracklist_insert_track (
  Tracklist * self,
  Track *     track,
  int         pos,
  int         publish_events);

/**
 * Removes a track from the Tracklist and the
 * TracklistSelections.
 *
 * @param free Free the track or not (free later).
 * @param publish_events Push a track deleted event
 *   to the UI.
 */
void
tracklist_remove_track (
  Tracklist * self,
  Track *     track,
  int         free,
  int         publish_events);

/**
 * Moves a track from its current position to the
 * position given by \p pos.
 *
 * @param publish_events Push UI update events or
 *   not.
 */
void
tracklist_move_track (
  Tracklist * self,
  Track *     track,
  int         pos,
  int         publish_events);

void
tracklist_append_track (Track *     track);

int
tracklist_get_track_pos (Track *     track);

ChordTrack *
tracklist_get_chord_track ();

int
tracklist_get_last_visible_pos ();

Track*
tracklist_get_last_visible_track ();

Track *
tracklist_get_first_visible_track ();

Track *
tracklist_get_prev_visible_track (Track * track);

Track *
tracklist_get_next_visible_track (Track * track);

/**
 * @}
 */

#endif
