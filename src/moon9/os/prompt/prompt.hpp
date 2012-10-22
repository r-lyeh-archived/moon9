/*
 * Simple prompt dialog (based on legolas558's code). MIT license.
 * - rlyeh
 */

#pragma once

#include <string>

std::string prompt( const std::string &title = std::string(), const std::string &current_value = std::string(), const std::string &caption = std::string() );
