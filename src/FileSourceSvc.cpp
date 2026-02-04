#include "FileSourceSvc.h"

#include <stdexcept>

using namespace NA64;

DECLARE_COMPONENT(FileSourceSvc)

StatusCode FileSourceSvc::initialize()
{
    StatusCode sc = Service::initialize();
    if (sc.isFailure()) return sc;

    if (infiles.size())
    {
        error() << "Empty source file list" << endmsg;
        StatusCode::FAILURE;
    }
    // TODO check all files

    currInput = infiles.begin();
    in_stream = nullptr;

    return StatusCode::SUCCESS;
}

StatusCode FileSourceSvc::finalize()
{
    if (in_stream != nullptr)
    {
        fclose(in_stream);
    }
    return Service::finalize();
}

std::optional<DAQEvent> FileSourceSvc::next()
{
    DAQEvent event;

    try
    {
        event.set_size(readUint32());
        event.set_magic(readUint32());
        event.set_headLen(readUint32());
        event.set_version(readUint32());

        if (! event.new_version())
        {
            //DATE_old format
            event.set_type(event.get_headLen());
            event.set_headLen(event.get_version());

            event.set_run_number(readUint32());
            event.set_burst_number(readUint32());
            event.set_number_in_run(readUint32());
            event.set_number_in_burst(readUint32());
            event.set_trigger_number(readUint32());

            uint32 tmpi = readUint32();  // dummy: fileSeqNb
            event.set_detectorId(readUint32(), readUint32(), readUint32());

            event.set_time(readUint32(), readUint32());
            event.set_error_code(readUint32());
            event.set_dead_time(readUint32(), readUint32());

            event.set_type_attribute(readUint32(), readUint32(), 0);
        }
        else
        {
            //DATE_36 format
            event.set_type(readUint32());

            event.set_run_number(readUint32());
            
            event.set_number_in_run(readUint32());

            uint32 tmpi = readUint32();
            event.set_burst_number((tmpi >> 20) & 0x00000fff);
            event.set_number_in_burst(tmpi & 0x000fffff);

            event.set_trigger_pattern(readUint32(), readUint32());
            tmpi = readUint32();  // dummy: event_detector_pattern

            event.set_type_attribute(readUint32(), readUint32(), readUint32());

            tmpi = readUint32();  // dummy: event_ldc_id
            tmpi = readUint32();  // dummy: event_gdc_id
            
            event.set_time(readUint32(), 0);
        }
    }
    catch (std::runtime_error exc)
    {
        error() << exc.what() << endmsg;
        return std::nullopt;
    }

    return event;
}

uint32 FileSourceSvc::readUint32()
{
    uint8 buffer[4];

    for (bool cycle = true; cycle;)
    {
        while (in_stream == nullptr)
        {
            debug() << "Reading data from " << *currInput << endmsg;

            if ((in_stream = fopen(currInput->c_str(), "rb")) == nullptr)
            {
                error() << "Cannot read data from " << *currInput << endmsg;
                currInput++;
                if (currInput == infiles.end())
                {
                    throw std::runtime_error("No more files");
                }
            }
        }

        int b_num = fread(buffer, 1, 4, in_stream);
        if (b_num == 0)
        {
            fclose(in_stream);
            in_stream = nullptr;
            continue;
        }
        else if (b_num != 4)
        {
            error() << "Cannot read data" << endmsg;
            throw std::runtime_error("Read data error");
        }
        cycle = false;
    }

    uint32 result = 0;
    result |= (buffer[0] << 24);
    result |= (buffer[1] << 16);
    result |= (buffer[2] << 8);
    result |= buffer[3];
    return result;
}

