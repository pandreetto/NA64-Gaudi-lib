#pragma once

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include <optional>
#include <utility>

namespace NA64
{

using uint8 = unsigned char;
using uint32 = unsigned int;

class GAUDI_API DAQEvent : public DataObject
{
public:
    static const uint32 SUPER_EVENT_MASK = 1u<<31;
    static const uint32 ATTR_SUPER_EVENT = 68;
    static const uint32 EVENT_MAGIC_NUMBER = 0xDA1E5AFE;

    bool operator == (DAQEvent& evn) const;

    bool have_subevents();
    uint32 get_trigger();

    inline void set_version(uint32 v) { version = v; }
    inline uint32 get_version() { return version; }
    inline bool new_version() { return version < 0xffff; }

    inline void set_size(uint32 s) { size = s; }
    inline uint32 get_size() { return size; }
    inline void set_magic(uint32 m) { magic = m; }
    inline uint32 get_magic() { return magic; }
    inline void set_headLen(uint32 l) { headLen = l; }
    inline uint32 get_headLen() { return headLen; }
    inline void set_type(uint32 t) { type = t; }
    inline uint32 get_type() { return type; }
    inline void set_run_number(uint32 n) { runNb = n; }
    inline uint32 get_run_number() { return runNb; }
    inline void set_burst_number(uint32 n) { burstNb = n; }
    inline uint32 get_burst_number() { return burstNb; }
    inline void set_number_in_run(uint32 n) { nbInRun = n; }
    inline uint32 get_number_in_run() { return nbInRun; }
    inline void set_number_in_burst(uint32 n) { nbInBurst = n; }
    inline uint32 get_number_in_burst() { return nbInBurst; }

    inline void set_trigger_number(uint32 n) { triggerNb = n; }
    uint32 get_trigger_number();

    inline void set_type_attribute(uint32 a, uint32 b, uint32 c) {
        typeAttribute[0] = a; typeAttribute[1] = b; typeAttribute[2] = c;
    }

    inline void set_time(uint32 t, uint32 us) { time = t; usec = us; }
    inline std::pair<time_t, uint32> get_time() {
        return std::pair<time_t, uint32>(time, usec);
    }
    inline void set_error_code(uint32 e) { errorCode = e; }
    uint32 get_error_code();
    inline void set_dead_time(uint32 t, uint32 us) { deadTime = t; deadTimeusec = us; }
    std::pair<time_t, uint32> get_dead_time();

    inline void set_detectorId(uint32 a, uint32 b, uint32 c) {
        detectorId[0] = a; detectorId[1] = b; detectorId[2] = c;
    }

    inline void set_trigger_pattern(uint32 a, uint32 b) {
        trigger_pattern[0] = a; trigger_pattern[1] = b;
    }


private:
    uint32 size;
    uint32 magic;
    uint32 headLen;
    uint32 version;
    uint32 type;
    uint32 runNb;
    uint32 burstNb;
    uint32 nbInRun;
    uint32 nbInBurst;
    uint32 triggerNb; 
    uint32 typeAttribute[3];
    uint32 time;
    uint32 usec;
    uint32 errorCode;
    uint32 deadTime;
    uint32 deadTimeusec;
    uint32 detectorId[3];
    uint32 trigger_pattern[2];
};

} // end namespace NA64
