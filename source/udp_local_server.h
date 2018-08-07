#ifndef UDP_LOCAL_SERVER_H
#define UDP_LOCAL_SERVER_H

/* THIS PROJECT INCLUDES */
#include "socket_control.h"

namespace protocol {
	namespace udp {
		class udp_local_server {
		public:
			void set_local_endpoint(
				std::string _local_address,
				uint16_t _local_port
			) {
				local_address = _local_address;
				local_port = _local_port;
			}

			void initialize(
			) {
				try {
					udp_endpoint local_endpoint =
						udp_endpoint(
							boost::asio::ip::address_v4::from_string(local_address),
							local_port
						);


					local_server_socket =
						boost::make_shared<udp_socket>(
							scheduler::get()->get_svc(),
							local_endpoint
							);
				}
				catch (const std::exception&) {}

				this->set_recv_from();
			}

			static udp_local_server* get(
			) {
				static udp_local_server* m = nullptr;
				if (!m)
					m = new udp_local_server();
				return m;
			}
		private:
			void set_recv_from(
			) {
				if (is_receiving)
					return;

				is_receiving = true;

				udp_endpoint_ptr last_received_endpoint =
					boost::make_shared<udp_endpoint>();

				auto bounded_function = boost::bind(
					&udp_local_server::on_downstream_recv,
					this,
					last_received_endpoint,
					boost::asio::placeholders::bytes_transferred,
					boost::asio::placeholders::error
				);

				auto boost_buffer =
					boost::asio::buffer(
						recv_buffer,
						sizeof(recv_buffer)
					);

				local_server_socket->async_receive_from(
					boost_buffer,
					*last_received_endpoint,
					bounded_function
				);
			}

			void check_send_to(
				buffer_ptr data_buffer,
				udp_endpoint_ptr last_received_endpoint
			) {
				auto boost_buffer =
					boost::asio::buffer(
						data_buffer->data(),
						data_buffer->size()
					);

				auto bounded_function =
					boost::bind(
						&udp_local_server::on_downstream_send,
						this,
						data_buffer,
						boost::asio::placeholders::bytes_transferred,
						boost::asio::placeholders::error
					);

				local_server_socket->async_send_to(
					boost_buffer,
					*last_received_endpoint,
					bounded_function
				);
			}

			void on_downstream_recv(
				udp_endpoint_ptr last_received_endpoint,
				const size_t& bytes_transferred,
				const boost_error& error_code
			) {
				is_receiving = false;

				if (!error_code) {
					buffer_ptr data_buffer =
						boost::make_shared<buffer_t>(recv_buffer, bytes_transferred);

					this->check_send_to(data_buffer, last_received_endpoint);
				}

				this->set_recv_from();
			}

			void on_downstream_send(
				buffer_ptr data_holder,
				const size_t& bytes_transferred,
				const boost_error& error_code
			) {
			}

		private:
			char recv_buffer[DEFAULT_BUFFER_SIZE];

			bool is_receiving;

			std::string local_address;
			uint16_t local_port;

			udp_socket_ptr local_server_socket;
		};
	}
}

#endif // !UDP_LOCAL_SERVER_H