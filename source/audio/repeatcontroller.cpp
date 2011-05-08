#include "repeatcontroller.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>
#include <powertabdocument/alternateending.h>

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;
using std::make_pair;

RepeatController::RepeatController(const Score* score) :
    score(score)
{
    indexRepeats();
}

/// Scans through the entire score and finds all of the pairs of repeat bars
void RepeatController::indexRepeats()
{
    // add start of score as the first repeat
    const SystemLocation scoreStartLocation(0, 0);
    repeats[scoreStartLocation] = Repeat(scoreStartLocation);

    for (size_t currentSystem = 0; currentSystem < score->GetSystemCount(); currentSystem++)
    {
        shared_ptr<System> system = score->GetSystem(currentSystem);

        vector<const Barline*> barlines;
        system->GetBarlines(barlines);

        for (size_t i = 0; i < barlines.size(); i++)
        {
            const Barline* currentBar = barlines.at(i);

            if (currentBar->IsRepeatStart())
            {
                const SystemLocation location(currentSystem,
                                              currentBar->GetPosition());

                repeats[location] = Repeat(location);
            }
            else if (currentBar->IsRepeatEnd())
            {
                // add to the end bar list for the active repeat group
                repeats.rbegin()->second.addRepeatEnd(SystemLocation(currentSystem, currentBar->GetPosition()),
                                                      RepeatEnd(currentBar->GetRepeatCount()));
            }
        }
    }

    // add alternate endings
    for (size_t i = 0; i < score->GetAlternateEndingCount(); i++)
    {
        shared_ptr<const AlternateEnding> altEnding = score->GetAlternateEnding(i);

        const SystemLocation altEndingLocation(altEnding->GetSystem(),
                                               altEnding->GetPosition());

        Repeat& activeRepeat = getPreviousRepeatGroup(altEndingLocation);
        activeRepeat.addAlternateEnding(altEnding);
    }
}

/// Returns the active repeat - the last repeat with a start bar before the given position
Repeat& RepeatController::getPreviousRepeatGroup(const SystemLocation& location)
{
    auto repeatGroup = repeats.upper_bound(location);
    if (repeatGroup != repeats.begin())
    {
        repeatGroup--;
    }

    return repeatGroup->second;
}

/// Checks if a repeat needs to be performed at the given system and position.
/// @return true If the playback position needs to be changed, and
/// updates the newSystem and newPos parameters with the new playback position
bool RepeatController::checkForRepeat(uint32_t currentSystem, uint32_t currentPos,
                                  uint32_t& newSystem, uint32_t& newPos)
{
    if (repeats.empty()) // no repeat events in the score
    {
        return false;
    }

    SystemLocation currentLocation(currentSystem, currentPos);
    Repeat& activeRepeat = getPreviousRepeatGroup(currentLocation);

    // perform the repeat
    SystemLocation newLocation = activeRepeat.performRepeat(currentLocation);

    if (newLocation == currentLocation) // if no position shift occurred
    {
        return false;
    }
    else
    {
        newSystem = newLocation.getSystemIndex();
        newPos = newLocation.getPositionIndex();
        return true;
    }
}
