#ifndef SETTINGS_H
#define SETTINGS_H

/* THIS PROJECT INCLUDES */
#include "tcp_local_server.h"
#include "logger.h"

/* JSON LIB INCLUDES */
//#include <lib_json/json.h>

namespace helper {
	class settings {
	public:
		void load_settings(
			std::string file_name
		) {
			/* auto json_value = Json::load_json_file(file_name);

			 if (json_value.isNull() || json_value.empty())
				 return;

			 std::string tcp_address = json_value["tcp_address"].asString();
			 uint16_t tcp_port = json_value["tcp_port"].asUInt();
			 uint32_t max_file_size = json_value["file_max_size"].asUInt();

			 logger::get();
			 logger::get().set_max_file_size(max_file_size);

			 protocol::tcp::tcp_local_server::get()->set_local_endpoint(
					 tcp_address,
					 tcp_port
			 );*/

			protocol::tcp::tcp_local_server::get()->set_local_endpoint(
				"0.0.0.0",
				9090
			);
		}

		static settings* get() {
			static settings*  m = nullptr;
			if (!m)
				m = new settings();
			return m;
		}
	private:

	};
}

#endif // !SETTINGS_H