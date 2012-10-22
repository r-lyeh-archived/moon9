#pragma once

#include <string>

void alert();
void alert( const std::string &t, const std::string &title = std::string() );
void alert( const      size_t &t, const std::string &title = std::string() );
void alert( const      double &t, const std::string &title = std::string() );
void alert( const       float &t, const std::string &title = std::string() );
void alert( const         int &t, const std::string &title = std::string() );
void alert( const        char &t, const std::string &title = std::string() );
void alert( const        bool &t, const std::string &title = std::string() );
