#ifndef SCHEDULER_H
#define SCHEDULER_H

/* THIS PROJECT INCLUDES */
#include "typedefs.h"

class scheduler {
public:
	scheduler() : current_work(current_service) {
	}

	void init() {
		auto bounded_function = boost::bind(
			&scheduler::run_service,
			this
		);

		boost::thread(bounded_function).detach();
	}

	boost::asio::io_service& get_svc() {
		return this->current_service;
	}

	static scheduler* get() {
		static scheduler* m = nullptr;
		if (!m)
			m = new scheduler();
		return m;
	}
private:
	void run_service() {
		while (true) {
			try {
				current_service.run();
			}
			catch (const std::exception&) {
				std::cout << "\n " << __FUNCTION__ << ": " << __LINE__;
			}

			/* TODO - AFTER REMOVE THIS SLEEP */
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

private:
	boost::asio::io_service current_service;
	boost::asio::io_service::work current_work;
};

#endif // !SCHEDULER_H
