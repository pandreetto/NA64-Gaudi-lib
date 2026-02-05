#include "DAQRawReader.h"

using namespace NA64;

DECLARE_COMPONENT(DAQRawReader)

DAQRawReader::DAQRawReader(const std::string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator),
    storeHandle()
{
    setProperty("Cardinality", 1).ignore();
}

StatusCode DAQRawReader::initialize()
{
    m_sourceSvc = service("DataGeneratorSvc");
    if (!m_sourceSvc)
    {
        error() << "Unable to locate IDataGeneratorSvc interface" << endmsg;
        return StatusCode::FAILURE;
    }
    storeHandle = std::make_unique<DataObjectHandle<DAQEvent>>(store_path, Gaudi::DataHandle::Writer, this);
    declareProperty( "treader_port", *(storeHandle) );

    return StatusCode::SUCCESS;
}

StatusCode DAQRawReader::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode DAQRawReader::execute()
{
    std::optional<DAQEvent> o_evn = m_sourceSvc->next();
    if (!o_evn)
    {
        info() << "Empty event" << endmsg;
        return StatusCode::SUCCESS;
    }

    auto event = o_evn.value();
    info() << "Registering (" << event.get_run_number() << " " << event.get_number_in_run() << endmsg;

    storeHandle->put(std::make_unique<DAQEvent>(event));
    return StatusCode::SUCCESS;
}

