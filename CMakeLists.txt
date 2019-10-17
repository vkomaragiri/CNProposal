cmake_minimum_required(VERSION 3.14)
project(CNProposal)

set(CMAKE_CXX_STANDARD 14)
set(COMMON_SOURCES CNProposal include/Utils.h include/Variable.h include/Function.h src/Utils.cpp include/MyTypes.h src/Function.cpp include/GM.h src/GM.cpp include/SamplingFunction.h include/MI.h include/Data.h src/MI.cpp include/MT.h src/MT.cpp include/HyperParameters.h src/HyperParameters.cpp src/Data.cpp include/GMSampler.h include/MTSampler.h include/BTP.h src/BTP.cpp include/myRandom.h src/MT_BTP.cpp src/myRandom.cpp)

# common include and link directories
include_directories(include /usr/local/Cellar/boost/1.71.0/include /usr/local/Cellar/python@2/2.7.16_1/Frameworks/Python.framework/Versions/2.7/include/python2.7/)
link_directories(/usr/local/Cellar/boost/1.71.0/lib/ /usr/local/Cellar/boost-python/1.71.0/lib /usr/local/Cellar/python@2/2.7.16_1/Frameworks/Python.framework/Versions/2.7/lib/)

# Learn Mixtures of Trees
add_executable(learnMT ${COMMON_SOURCES} target_src/main_learn_mt.cpp)
# Sample Mixtures of Trees
add_executable(sampleMT ${COMMON_SOURCES} target_src/main_sample_mt.cpp)
# Perform Exact Inference
add_executable(exactInf ${COMMON_SOURCES} target_src/main_exact_inf.cpp)
# Perform Approximate Sampling Based Inference
add_executable(sampleInf ${COMMON_SOURCES} target_src/main_sample_inf.cpp)


target_link_libraries(learnMT boost_program_options-mt)
target_link_libraries(sampleMT boost_program_options-mt)
target_link_libraries(exactInf boost_program_options-mt)
target_link_libraries(sampleInf boost_program_options-mt)


# Find default python libraries and interpreter
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)

# Build and link the pylib module
add_library(GMLib SHARED ${COMMON_SOURCES} src/PythonInterface.cpp)
target_link_libraries(GMLib boost_program_options-mt boost_python27-mt python2.7 boost_numpy27-mt )


# Tweaks the name of the library to match what Python expects
set_target_properties(GMLib PROPERTIES SUFFIX .so)
set_target_properties(GMLib PROPERTIES PREFIX "")
message(${CMAKE_CXX_FLAGS_RELEASE})


