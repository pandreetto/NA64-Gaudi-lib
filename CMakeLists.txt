cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(NA64GaudiLib)

find_package(Gaudi CONFIG REQUIRED)

include_directories(./include)

gaudi_add_module(GaudiMasterAlgo
                 SOURCES src/FileSourceSvc.cpp
		         src/DAQEvent.cpp
			 src/DAQRawReader.cpp
                 LINK GaudiKernel)

install(DIRECTORY python/NA64GaudiLib
        DESTINATION ${GAUDI_INSTALL_PYTHONDIR}
        FILES_MATCHING PATTERN "*.py")
