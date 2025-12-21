/*
Original MMO Client/Server Framework using ASIO
by David Barr (javidx9) – OLC-3 License

This file was implemented alongside the tutorial for learning purposes.
Code was written in parallel with the tutorial, and debugging, understanding,
and integration into the Lan Chart App project were done by Hamdan, 2025.

See LICENSE file for full OLC-3 license details.
*/

#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
