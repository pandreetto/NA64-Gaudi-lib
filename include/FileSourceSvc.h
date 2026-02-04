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
    uint32 readUint32();

    Gaudi::Property<std::vector<std::string>> infiles { this, "inputFiles", {}, "List of data files" };

    SmartIF<IIncidentSvc> m_incidentSvc;

    std::vector<std::string>::iterator currInput;
    FILE* in_stream;
};

} // end namespace NA64
