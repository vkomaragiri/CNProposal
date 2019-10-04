cmake_minimum_required(VERSION 3.14)
project(CNProposal)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG  "-O3")
include_directories(include /usr/local/Cellar/boost/1.71.0/include)
add_executable(CNProposal include/Utils.h include/Variable.h include/Function.h src/Utils.cpp include/MyTypes.h src/Function.cpp include/GM.h src/GM.cpp src/main.cpp include/SamplingFunction.h include/MI.h include/Data.h src/MI.cpp include/MT.h src/MT.cpp include/HyperParameters.h src/HyperParameters.cpp src/Data.cpp include/GMSampler.h include/MTSampler.h include/BTP.h src/BTP.cpp include/myRandom.h src/MT_BTP.cpp)

#project(tutorial)

# Find default python libraries and interpreter
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)


include_directories(include /usr/local/Cellar/boost/1.71.0/include ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})
link_directories(/usr/local/Cellar/boost-python/1.71.0/lib /usr/local/Cellar/python@2/2.7.16_1/Frameworks/Python.framework/Versions/2.7/lib/)

# Build and link the pylib module
add_library(GMLib SHARED src/PythonInterface.cpp include/Utils.h include/Variable.h include/Function.h src/Utils.cpp include/MyTypes.h src/Function.cpp include/GM.h src/GM.cpp src/main.cpp include/SamplingFunction.h include/MI.h include/Data.h src/MI.cpp include/MT.h src/MT.cpp include/HyperParameters.h src/HyperParameters.cpp src/Data.cpp include/GMSampler.h include/MTSampler.h include/BTP.h src/BTP.cpp include/myRandom.h src/MT_BTP.cpp)

target_link_libraries(GMLib boost_python27-mt python2.7 boost_numpy27-mt)


# Tweaks the name of the library to match what Python expects
set_target_properties(GMLib PROPERTIES SUFFIX .so)
set_target_properties(GMLib PROPERTIES PREFIX "")
message(${CMAKE_CXX_FLAGS_RELEASE})


