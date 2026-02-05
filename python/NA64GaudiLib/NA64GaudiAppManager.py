from Configurables import ApplicationMgr
from Configurables import AvalancheSchedulerSvc
from Configurables import HiveWhiteBoard
from Configurables import HiveSlimEventLoopMgr
from Configurables import AlgResourcePool

from Configurables import Gaudi__Sequencer as Sequencer

from Configurables import NA64__DAQRawReader

from Gaudi.Configuration import DEBUG, WARNING, INFO

class AppManager:

    def __init__(self):
        self.event_slots = 10
        self.threadpool_size = 10
        self.max_events = 100
        self.algorithms = list()

    def setup(self, **kwargs):

        for item in [ 'event_slots', 'threadpool_size', 'max_events' ]:
            if item in kwargs:
                setattr(self, item, int(kwargs[item]))

        missing_reader = True
        if 'algorithms' in kwargs:
            for item in kwargs['algorithms']:
                missing_reader = missing_reader and (not isinstance(item,NA64__DAQRawReader))
                self.algorithms.append(item)
        if missing_reader:
            self.algorithms.insert(0, NA64__DAQRawReader("NA64__DAQRawReader"))

        whiteboard = HiveWhiteBoard("EventDataSvc", EventSlots = self.event_slots)
        scheduler = AvalancheSchedulerSvc(ThreadPoolSize = self.threadpool_size,
                                          OutputLevel = WARNING)

        slimeventloopmgr = HiveSlimEventLoopMgr(SchedulerName = "AvalancheSchedulerSvc",
                                                OutputLevel = WARNING)
        AlgResourcePool(OutputLevel = WARNING)

        ApplicationMgr(JobOptionsType = 'NONE',
                       EvtSel = 'NONE',
                       EvtMax = self.max_events,
                       ExtSvc = [ whiteboard ],
                       EventLoop = slimeventloopmgr,
                       MessageSvcType = "InertMessageSvc",
                       TopAlg = self.algorithms)

