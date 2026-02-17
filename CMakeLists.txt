cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(NA64GaudiLib)

find_package(EXPAT REQUIRED)

find_package(Gaudi CONFIG REQUIRED)

find_package(DATEMONITORING CONFIG REQUIRED)

find_package(CoralDAQ CONFIG REQUIRED)

include_directories(./include
                    ${EXPAT_INCLUDE_DIRS}
                    ${DATEMONITORING_INCLUDE_DIRS}
                    ${CoralDAQ_INCLUDE_DIRS})

gaudi_add_module(GaudiMasterAlgo
                 SOURCES src/FileSourceSvc.cpp
		                 src/DAQEvent.cpp
        		         src/DAQRawReader.cpp
                 LINK GaudiKernel
                      ${DATEMONITORING_LIBRARIES}
                      ${EXPAT_LIBRARIES}
                      ${CoralDAQ_LIBRARIES})

install(DIRECTORY python/NA64GaudiLib
        DESTINATION ${GAUDI_INSTALL_PYTHONDIR}
        FILES_MATCHING PATTERN "*.py")
