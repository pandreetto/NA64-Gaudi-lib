#pragma once

#include "GaudiKernel/Algorithm.h"
#include <GaudiKernel/DataObjectHandle.h>

#include "AbstractSourceSvc.h"

#include <memory>

namespace NA64
{

class DAQRawReader : public Algorithm
{
public:
    DAQRawReader(const std::string& name, ISvcLocator* pSvcLocator);
    DAQRawReader(const DAQRawReader&) = delete;
    virtual ~DAQRawReader() override {}

    StatusCode initialize() override;
    StatusCode finalize() override;
    StatusCode execute() override;

    DAQRawReader& operator=(const DAQRawReader&) = delete;

    bool isReEntrant() const override { return false; }

private:

    Gaudi::Property<std::string> store_path { this, "store_path", "/Event/DAQRaw", "Location in the store for data" };
    std::unique_ptr<DataObjectHandle<DAQEvent>> storeHandle;

    SmartIF<ISourceSvc> m_sourceSvc;
};

} // end namespace NA64
