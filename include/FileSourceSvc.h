#pragma once

#include "Gaudi/Property.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Service.h"

#include "AbstractSourceSvc.h"

#include <cstdio>
#include <string>
#include <vector>

namespace NA64
{

class FileSourceSvc : public extends<Service, ISourceSvc, IIncidentListener>
{
public:
    using extends::extends;

    StatusCode initialize() override;
    StatusCode finalize() override;

    std::optional<DAQEvent> next() override;

    const static int ReadMinimum = 16;

protected:
    void handle(const Incident& incident) override {}
    std::vector<uint32> readUint32(unsigned int n_word);
    void parse_chip_list(const std::vector<uint32>& payload, bool new_ver);

    Gaudi::Property<std::vector<std::string>> infiles { this, "inputFiles", {}, "List of data files" };
    Gaudi::Property<size_t> max_evn_num { this, "maxEventNumber", 0, "Maximum number of events to process" };

    SmartIF<IIncidentSvc> m_incidentSvc;

    std::vector<std::string>::iterator currInput;
    FILE* in_stream;
    size_t evn_counter;
};

} // end namespace NA64
