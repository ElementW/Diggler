set(DIGGLER_ENABLE_JEMALLOC FALSE CACHE BOOL "Enable jemalloc")
set(DIGGLER_ENABLE_MEMORY_TRACKER FALSE CACHE BOOL "Enable memory tracker")

set(CSD ${CMAKE_CURRENT_SOURCE_DIR})
diggler_add_sources(
  ${CSD}/BitmapDumper.cpp
  ${CSD}/ColorUtil.cpp
  ${CSD}/Encoding.cpp
  ${CSD}/Log.cpp
  ${CSD}/logging/AnsiConsoleLogger.cpp
  ${CSD}/logging/Logger.cpp
  ${CSD}/logging/LogInput.cpp
  ${CSD}/logging/StdoutLogger.cpp
  ${CSD}/StringUtil.cpp
  ${CSD}/TexturePacker.cpp
  ${CSD}/Tipsify.cpp
)

if (DIGGLER_ENABLE_MEMORY_TRACKER)
  diggler_add_definition(
    -DDIGGLER_ENABLE_MEMORY_TRACKER=1
  )
  diggler_add_sources(
    ${CSD}/MemoryTracker.cpp
  )
  diggler_link_libraries(
    -Wl,-wrap,malloc
    -Wl,-wrap,calloc
    -Wl,-wrap,realloc
    -Wl,-wrap,free
    -Wl,-wrap,posix_memalign
  )
else()
  diggler_add_sources(
    ${CSD}/MemoryTracker.disabled.cpp
  )
endif()

if (DIGGLER_ENABLE_JEMALLOC)
  find_package(Jemalloc REQUIRED)

  diggler_add_definition(
    -DDIGGLER_ENABLE_JEMALLOC=1
  )
  diggler_link_libraries(
    ${JEMALLOC_LIBRARIES}
  )
endif()
