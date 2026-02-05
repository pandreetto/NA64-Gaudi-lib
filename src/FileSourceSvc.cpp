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
        auto wbuf = readUint32(4);
        event.set_size(wbuf[0]);
        event.set_magic(wbuf[1]);
        event.set_headLen(wbuf[2]);
        event.set_version(wbuf[3]);

        if (! event.new_version())
        {
            //DATE_old format
            event.set_type(event.get_headLen());
            event.set_headLen(event.get_version());

            auto wbuf2 = readUint32(16);
            event.set_run_number(wbuf2[0]);
            event.set_burst_number(wbuf2[1]);
            event.set_number_in_run(wbuf2[2]);
            event.set_number_in_burst(wbuf2[3]);
            event.set_trigger_number(wbuf2[4]);

            uint32 tmpi = wbuf2[5];  // dummy: fileSeqNb
            event.set_detectorId(wbuf2[6], wbuf2[7], wbuf2[8]);

            event.set_time(wbuf2[9], wbuf2[10]);
            event.set_error_code(wbuf2[11]);
            event.set_dead_time(wbuf2[12], wbuf2[13]);

            event.set_type_attribute(wbuf2[14], wbuf2[15], 0);
        }
        else
        {
            //DATE_36 format
            auto wbuf2 = readUint32(13);
            event.set_type(wbuf2[0]);

            event.set_run_number(wbuf2[1]);
            
            event.set_number_in_run(wbuf2[2]);

            uint32 tmpi = wbuf2[3];
            event.set_burst_number((tmpi >> 20) & 0x00000fff);
            event.set_number_in_burst(tmpi & 0x000fffff);

            event.set_trigger_pattern(wbuf2[4], wbuf2[5]);
            tmpi = wbuf2[6];  // dummy: event_detector_pattern

            event.set_type_attribute(wbuf2[7], wbuf2[8], wbuf2[9]);

            tmpi = wbuf2[10];  // dummy: event_ldc_id
            tmpi = wbuf2[11];  // dummy: event_gdc_id
            
            event.set_time(wbuf2[12], 0);
        }
    }
    catch (std::runtime_error exc)
    {
        error() << exc.what() << endmsg;
        return std::nullopt;
    }

    return event;
}

std::vector<uint32> FileSourceSvc::readUint32(unsigned int n_word)
{
    unsigned int n_bytes = 4 * n_word;
    uint8 buffer[n_bytes];

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

        int b_num = fread(buffer, 1, n_bytes, in_stream);
        if (b_num == 0)
        {
            fclose(in_stream);
            in_stream = nullptr;
            continue;
        }
        else if (b_num != n_bytes)
        {
            error() << "Cannot read data" << endmsg;
            throw std::runtime_error("Read data error");
        }
        cycle = false;
    }

    std::vector<uint32> result { n_word };
    result.assign(n_word, 0);
    for (int k = 0; k < n_word; k++)
    {
        result[k] |= (buffer[0] << 24);
        result[k] |= (buffer[1] << 16);
        result[k] |= (buffer[2] << 8);
        result[k] |= buffer[3];
    }
    return result;
}

