#include "FileSourceSvc.h"

#include "SLink.h"

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
    evn_counter = 0;

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

/* *************************************************************************
 *
 * ************************************************************************* */
std::optional<DAQEvent> FileSourceSvc::next()
{
    if (max_evn_num > 0 and evn_counter == max_evn_num)
    {
        return std::nullopt;
    }
    evn_counter++;

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

            if (event.get_headLen() != 64)
            {
                // TODO remove after tests (head len is in bytes)
                throw std::runtime_error("Critical error: header length mismatch");
            }
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

            if (event.get_headLen() != 52)
            {
                // TODO remove after tests (head len is in bytes)
                throw std::runtime_error("Critical error: header length mismatch");
            }
        }

        if (event.get_magic() != DAQEvent::EVENT_MAGIC_NUMBER)
        {
            error() << "Magic number was not found" << endmsg;
            return std::nullopt;
        }
        if ((event.get_size() % 4) != 0 or event.get_size() < event.get_headLen())
        {
            error() << "Bad event length" << endmsg;
            return std::nullopt;
        }

        uint32 payload_size = (event.get_size() - event.get_headLen()) / 4;
        auto payload = readUint32(payload_size);

        uint32 mtype = event.get_type() & DAQEvent::EVENT_TYPE_MASK;
        if (mtype == DAQEvent::PHYSICS_EVENT or mtype == DAQEvent::CALIBRATION_EVENT 
            or mtype == DAQEvent::END_OF_BURST)
        {
            parse_chip_list(payload, event.new_version());
        }
    }
    catch (std::runtime_error exc)
    {
        error() << exc.what() << endmsg;
        return std::nullopt;
    }

    return event;
}

/* *************************************************************************
 *
 * ************************************************************************* */
void FileSourceSvc::parse_chip_list(const std::vector<uint32>& payload, bool new_ver)
{
    for(int offset = 0; offset < payload.size(); )
    {
        uint32 s_offset = 0;
        // Equipment section
        if (new_ver)
        {
            uint32 equipSize = payload[offset];    // in bytes
            uint32 equipType = payload[offset + 1];
            uint32 equipID = payload[offset + 2];

            s_offset = offset + 7;
            offset += (equipSize / 4);
        }
        else
        {
            uint32 equipExtLen = (payload[offset] & 0xffff0000) >> 16;
            uint32 equipType = payload[offset] & 0x0000ffff;
            uint32 equipID = payload[offset + 1] & 0x0000ffff;
            uint32 equipRawLen = payload[offset + 2];

            s_offset = offset + (12 + equipExtLen) / 4;
            offset += (12 + equipExtLen + equipRawLen) / 4;
        }

        // SLink section
        CS::SLink slink;
    
        slink.SetEventSize(payload[s_offset] & 0x0000ffff);
        slink.SetSourceID((payload[s_offset] & 0x03ff0000) >> 16);
        slink.SetEventType((payload[s_offset] & 0x7c000000) >> 26);
        slink.SetError((payload[s_offset] & 0x80000000) >> 31);
        slink.SetEventNumber(payload[s_offset + 1] & 0x000fffff);
        slink.SetSpillNumber((payload[s_offset + 1] & 0x7ff00000) >> 20);
        slink.SetStat((payload[s_offset + 1] & 0x80000000) >> 31);
        if (slink.IsSLinkMultiplexer())
        {
        }
        else
        {
            slink.SetStatus(payload[s_offset + 2] & 0x000000ff);
            slink.SetTCSError((payload[s_offset + 2] & 0x0000ff00) >> 8);
            slink.SetErrorsCounter((payload[s_offset + 2] & 0x00ff0000) >> 16);
            slink.SetFormat((payload[s_offset + 2] & 0xff000000) >> 24);
            // TODO create chip
        }
    }
}

/* *************************************************************************
 *
 * ************************************************************************* */
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

