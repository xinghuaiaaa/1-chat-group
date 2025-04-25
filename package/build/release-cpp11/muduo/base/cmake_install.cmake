# Install script for directory: /root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/root/hzhdata/2025-bigproject/1-chat-web/package/build/release-install-cpp11")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/root/hzhdata/2025-bigproject/1-chat-web/package/build/release-cpp11/lib/libmuduo_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/AsyncLogging.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Atomic.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/BlockingQueue.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/BoundedBlockingQueue.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Condition.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/CountDownLatch.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/CurrentThread.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Date.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Exception.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/FileUtil.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/GzipFile.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/LogFile.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/LogStream.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Logging.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Mutex.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/ProcessInfo.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Singleton.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/StringPiece.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Thread.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/ThreadLocal.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/ThreadLocalSingleton.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/ThreadPool.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/TimeZone.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Timestamp.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/Types.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/WeakCallback.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/copyable.h"
    "/root/hzhdata/2025-bigproject/1-chat-web/package/muduo/muduo/base/noncopyable.h"
    )
endif()

