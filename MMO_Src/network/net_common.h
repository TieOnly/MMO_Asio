#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <chrono>
#include <cstdint>
#include <assert.h>
#include <iostream>

// #ifdef _WIN32
// #define _WIN32_WINNT 0x0A00
// #endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
