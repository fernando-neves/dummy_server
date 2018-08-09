#ifndef SETTINGS_H
#define SETTINGS_H

/* THIS PROJECT INCLUDES */
#include "tcp_local_server.h"
#include "udp_local_server.h"

#include "logger.h"

namespace helper {
	class settings {
	public:
		void load_settings(
			std::string file_name
		) {
			protocol::tcp::tcp_local_server::get()->set_local_endpoint("0.0.0.0", DEFAULT_PORT_TCP);
			protocol::udp::udp_local_server::get()->set_local_endpoint("0.0.0.0", DEFAULT_PORT_UDP);
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