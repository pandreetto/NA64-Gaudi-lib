#pragma once

#include "GaudiKernel/IDataProviderSvc.h"

#include "DAQEvent.h"

#include <optional>

namespace NA64
{

class ISourceSvc : virtual public IInterface
{
public:
    DeclareInterfaceID(ISourceSvc, 1, 0);
    virtual std::optional<DAQEvent> next() = 0;
};

} // end namespace NA64
