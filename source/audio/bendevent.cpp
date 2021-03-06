/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "bendevent.h"

#include <audio/midioutputdevice.h>
#include <score/generalmidi.h>

const uint8_t BendEvent::PITCH_BEND_RANGE = 24;
const uint8_t BendEvent::DEFAULT_BEND = 64;

/// Pitch bend amount to bend a note by a quarter tone.
const double BendEvent::BEND_QUARTER_TONE =
    (Midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL - BendEvent::DEFAULT_BEND) /
    (2.0 * BendEvent::PITCH_BEND_RANGE);

BendEvent::BendEvent(int channel, double startTime, int position,
                     int system, uint8_t bendAmount)
    : MidiEvent(channel, startTime, 0, position, system),
      myBendAmount(bendAmount)
{
}

void BendEvent::performEvent(MidiOutputDevice &device) const
{
    device.setPitchBend(myChannel, myBendAmount);
}
