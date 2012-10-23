#pragma once

#include <iostream>

// api

void attach( std::ostream &os, void (*custom_stream_callback)( bool open, bool feed, bool close, const std::string &line ) );
void detach( std::ostream &os, void (*custom_stream_callback)( bool open, bool feed, bool close, const std::string &line ) );
void detach( std::ostream &os );

// extra api

std::ostream &create_ostream( void (*custom_stream_callback)( bool open, bool feed, bool close, const std::string &line ) );
