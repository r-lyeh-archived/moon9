/* Simple http request class
 * Based on code by Vijay Mathew Pandyalakal
 *
 * - rlyeh
 */

#pragma once

#include <map>
#include <string>

namespace http
{
	class request
	{
		public:

			std::map<std::string,std::string> vars;
			std::string host, path, response, error;

			 request();
			~request();

			bool connect();
			bool send();

		private:

			struct impl;
			impl *pImpl;

			// disable copy & assignment
			request( const request &other ) {}
			request &operator =( const request &other ) {}
	};

	std::string download( const std::string &url );
}
