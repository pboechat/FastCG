cmake_minimum_required(VERSION 2.8)

project (log4cpp_static)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else(WIN32)
  if(APPLE)
    add_definitions(-DNDEBUG -DLOG4CPP_HAVE_SSTREAM)
  else(APPLE)
    add_definitions(-pthread -DNDEBUG -DLOG4CPP_HAVE_SSTREAM)
  endif(APPLE)
endif(WIN32)

add_library(log4cpp_static
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Appender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/AppenderSkeleton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/AppendersFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/BufferingAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FactoryParams.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LayoutsFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LevelEvaluator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Localtime.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PassThroughLayout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/TriggeringEventEvaluatorFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LayoutAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FileAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/RollingFileAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FixedContextCategory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/IdsaAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/OstreamAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/StringQueueAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SyslogAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/RemoteSyslogAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SimpleLayout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/BasicLayout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PatternLayout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Category.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CategoryStream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HierarchyMaintainer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Configurator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/BasicConfigurator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SimpleConfigurator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PropertyConfigurator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PropertyConfiguratorImpl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LoggingEvent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Priority.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/NDC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Filter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/TimeStamp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/StringUtil.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Properties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Win32DebugAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/NTEventLogAppender.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DllMain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DummyThreads.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSThreads.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/OmniThreads.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PThreads.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PortabilityImpl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/AbortAppender.cpp
)

if(WIN32)
  target_link_libraries(log4cpp_static kernel32 user32 ws2_32 advapi32)
  set_target_properties(log4cpp_static PROPERTIES LINK_FLAGS /NODEFAULTLIB:msvcrt)
endif(WIN32)