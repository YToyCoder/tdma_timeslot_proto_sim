cmake_minimum_required(VERSION 3.14.0)

set(CMKAE_C_COMPILER "/usr/bin/gcc")

project(timeslot_sm)

add_definitions(-g)

add_executable(timeslot_sm)

target_sources(timeslot_sm PRIVATE 
  src/main.c 
  src/timeslot/timeslot_signal.c
  src/timeslot/timeslot.c
  src/timeslot/timeslot_tb.c
  src/timeslot/tr_tb.c
  src/com/data_channel.c
  src/com/data_channel_config.c
  src/util/cJSON/cJSON.c
  src/util/cJSON/cJSON_Utils.c
)

if (NOT DEFINED ENV{LIBUV_HOME})
  message(FATAL_ERROR "LIBUV_HOME not set")
endif()

target_link_directories(timeslot_sm PRIVATE $ENV{LIBUV_HOME})
target_include_directories(timeslot_sm PRIVATE src "$ENV{LIBUV_HOME}/include")
message($ENV{LIBUV_HOME}/include)
target_link_libraries(timeslot_sm PRIVATE uv)

