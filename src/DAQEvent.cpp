#include "DAQEvent.h"

#include <stdexcept>

using namespace NA64;

bool DAQEvent::operator== (DAQEvent& evn) const
{
    return version == evn.version &&
           magic == evn.magic &&
           burstNb == evn.burstNb &&
           runNb == evn.runNb &&
           nbInRun == evn.nbInRun &&
           nbInBurst == evn.nbInBurst;
}

bool DAQEvent::have_subevents()
{
    if (new_version())
    {
        return typeAttribute[ATTR_SUPER_EVENT / 32] & (1 << (ATTR_SUPER_EVENT & 0x1f));
    }
    return detectorId[0] & SUPER_EVENT_MASK || detectorId[2] & SUPER_EVENT_MASK;
}

uint32 DAQEvent::get_trigger()
{
    if (new_version())
    {
        return (trigger_pattern[0] >> 1) + (trigger_pattern[1] << 31);
    }
    return typeAttribute[1];
}

uint32 DAQEvent::get_trigger_number()
{
    if (new_version())
    {
        throw std::runtime_error("Unsupported field: trigger number");
    }
    return triggerNb;
}

uint32 DAQEvent::get_error_code()
{
    if (new_version())
    {
        throw std::runtime_error("Unsupported field: error code");
    }
    return errorCode;
}

std::pair<time_t, uint32> DAQEvent::get_dead_time()
{
    if (new_version())
    {
        throw std::runtime_error("Unsupported field: dead time");
    }
    return std::pair<time_t, uint32>(deadTime, deadTimeusec);;
}
