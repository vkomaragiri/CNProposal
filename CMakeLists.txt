cmake_minimum_required(VERSION 3.14)
project(CNProposal)

set(CMAKE_CXX_STANDARD 14)
set(COMMON_SOURCES CNProposal include/Utils.h include/Variable.h include/Function.h include/MyTypes.h include/GM.h include/SamplingFunction.h include/MI.h include/Data.h include/MT.h include/HyperParameters.h include/GMSampler.h include/MTSampler.h include/BTP.h include/myRandom.h src/BTP.cpp src/Utils.cpp src/Function.cpp src/GM.cpp src/MI.cpp src/MT.cpp src/HyperParameters.cpp src/Data.cpp src/MT_BTP.cpp src/myRandom.cpp)


FIND_PACKAGE(PythonInterp)

if (PYTHONINTERP_FOUND)
    if (UNIX AND NOT APPLE)
        if (PYTHON_VERSION_MAJOR EQUAL 3)
            FIND_PACKAGE(Boost COMPONENTS python${PYTHON_VERSION_SUFFIX})
            FIND_PACKAGE(PythonInterp 3)
            FIND_PACKAGE(PythonLibs 3 REQUIRED)
            find_package(boost_numpy REQUIRED)
        else()
            FIND_PACKAGE(Boost COMPONENTS python)
            FIND_PACKAGE(PythonInterp)
            FIND_PACKAGE(PythonLibs REQUIRED)
        endif()
    else()
        if (PYTHON_VERSION_MAJOR EQUAL 3)
            FIND_PACKAGE(Boost COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
            FIND_PACKAGE(PythonInterp 3)
            FIND_PACKAGE(PythonLibs 3 REQUIRED)
        else()
            FIND_PACKAGE(Boost COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
            FIND_PACKAGE(PythonInterp)
            FIND_PACKAGE(PythonLibs REQUIRED)
        endif()
    endif()
else()
    message("Python not found")
endif()

message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")
message(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")

# common include and link directories
#include_directories(include /usr/local/Cellar/boost/1.71.0/include /usr/local/Cellar/python@2/2.7.16_1/Frameworks/Python.framework/Versions/2.7/include/python2.7/)
#link_directories(/usr/local/Cellar/boost/1.71.0/lib/ /usr/local/Cellar/boost-python/1.71.0/lib /usr/local/Cellar/python@2/2.7.16_1/Frameworks/Python.framework/Versions/2.7/lib/)

INCLUDE_DIRECTORIES(include ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

# Learn Mixtures of Trees
add_executable(learnMT ${COMMON_SOURCES} target_src/main_learn_mt.cpp)
# Sample Mixtures of Trees
add_executable(sampleMT ${COMMON_SOURCES} target_src/main_sample_mt.cpp)
# Perform Exact Inference
add_executable(exactInf ${COMMON_SOURCES} target_src/main_exact_inf_mt.cpp)
# Perform Approximate Sampling Based Inference
add_executable(sampleInf ${COMMON_SOURCES} target_src/main_sample_inf.cpp)

# Sample Bayesian Networks
add_executable(sampleBN ${COMMON_SOURCES} target_src/main_sample_bn.cpp)
# Generate evidence from leaves in BN
add_executable(genEvid ${COMMON_SOURCES} target_src/main_gen_evid.cc)
# Learn cpts from data for BN
add_executable(cptBN ${COMMON_SOURCES} target_src/main_bn_sample_cpt.cpp)

# Check MTS
add_executable(mtsCheck ${COMMON_SOURCES} target_src/main_check_mts.cpp)

target_link_libraries(learnMT boost_program_options)
target_link_libraries(sampleMT boost_program_options)
target_link_libraries(exactInf boost_program_options)
target_link_libraries(sampleInf boost_program_options)
target_link_libraries(sampleBN boost_program_options)

target_link_libraries(genEvid boost_program_options)
target_link_libraries(cptBN boost_program_options)
target_link_libraries(mtsCheck boost_program_options)

# Find default python libraries and interpreter
#find_package(PythonInterp REQUIRED)
#find_package(PythonLibs REQUIRED)



# Build and link the pylib module
add_library(GMLib SHARED ${COMMON_SOURCES} src/PythonInterface.cpp)
include_directories(GMLib ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
#target_link_libraries(GMLib boost_program_options boost_python3 python3.6m boost_numpy3)
target_link_libraries(GMLib boost_program_options ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} boost_numpy3)

# Tweaks the name of the library to match what Python expects
set_target_properties(GMLib PROPERTIES SUFFIX .so)
set_target_properties(GMLib PROPERTIES PREFIX "")
message(${CMAKE_CXX_FLAGS_RELEASE})