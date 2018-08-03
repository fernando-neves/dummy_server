#ifndef LOGGER_H
#define LOGGER_H

/* THIS PROJECT INCLUDES */
#include <helper/scheduler.h>

/* C++ 11 INCLUDES */
#include <mutex>
#include <iosfwd>
#include <condition_variable>

/* BOOST 1.58.0 INCLUDES */
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread.hpp>

/* MACROS */
#define LOG_BASE_PATH ""
#define LOG_BASE_NAME "socket_manager.log"
#define MAX_FILE_SIZE 104857600

namespace helper {
    class logger {
    public:
        logger() {
            this->file_path = LOG_BASE_PATH;
            this->file_name = LOG_BASE_NAME;

            this->max_file_size = MAX_FILE_SIZE;

            is_writing = false;

            this->check_generate_path();
            this->init_thread();
        }

        void set_max_file_size(
                uint32_t max_size
        ) {
            this->max_file_size = max_size;
        }

        template<typename T>
        logger& operator << (T in) {
            std::ostringstream ss;
            ss << in;

            add_log(ss.str());
            cv.notify_all();

            return *this;
        }

        static logger& get() {
            static logger* m = nullptr;
            if (!m)
                m = new logger();
            return *m;
        }

    private:
        void add_log(
                std::string current_log
        ) {
            std::lock_guard<std::recursive_mutex> _guard(mtx_access);

            if (log_queue.size() > 50 * 1000)
                return;

            log_queue.push_back(current_log);
        }

        void set_writing(
                bool _in
        ) {
            std::lock_guard <std::recursive_mutex>_guard(mtx_access);
            is_writing = _in;
        }

        std::shared_ptr<boost::filesystem::ofstream> open_file(
        ) {
            std::shared_ptr<boost::filesystem::ofstream> _file =
                    std::shared_ptr<boost::filesystem::ofstream>(
                            new boost::filesystem::ofstream(
                                    get_file_full_path(),
                                    std::ios::ios_base::app
                            )
                    );

            if (!_file.get())
                return nullptr;

            return _file;
        }

        bool write_to_file(
                std::string log_str
        ) {
            check_move_file();
            auto _file = open_file();
            if (!_file)
                return false;
            return write_content(_file, log_str);
        }

        bool write_content(
                std::shared_ptr<boost::filesystem::ofstream> _ofs,
                std::string content
        ) {
            *_ofs << content;
            return !_ofs->bad();
        }

        void check_move_file(
        ) {
            auto current_size = get_current_file_size();
            if (current_size < max_file_size)
                return;

            std::remove((get_file_full_path() + "." + std::to_string(5)).data());

            for (int i = 5 - 1; i > 0; i--) {
                std::string old_fpath = get_file_full_path() + "." + std::to_string(i);
                std::string new_fpath = get_file_full_path() + "." + std::to_string(i + 1);
                std::rename(old_fpath.data(), new_fpath.data());
            }

            std::string old_fpath = get_file_full_path();
            std::string new_fpath = get_file_full_path() + ".1";
            std::rename(old_fpath.data(), new_fpath.data());
        }

        std::string get_file_full_path(
        ) {
            std::lock_guard <std::recursive_mutex>_guard(mtx_access);
            return (boost::filesystem::path(this->file_path) / boost::filesystem::path(this->file_name)).string();
        }

        uint64_t get_current_file_size(
        ){
            std::string current_path =
                    get_file_full_path();

            if (!boost::filesystem::exists(current_path))
                return 0;

            uint64_t file_size =
                    boost::filesystem::file_size(current_path);

            return file_size;
        }

        void writer_thread(
        ) {
            for (;;) {
                if (wait_at_write()) {
                    mtx_access.lock();
                    std::vector<std::string> _queue = get_queue();
                    mtx_access.unlock();

                    set_writing(_queue.size() != 0);

                    while (_queue.size()) {
                        write_to_file(_queue);
                        std::this_thread::sleep_for(std::chrono::milliseconds(2));
                    }

                    set_writing(false);
                }
            }
        }

        void write_to_file(
                std::vector<std::string>& log_str
        ) {
            check_move_file();

            auto _file = open_file();
            if (!_file)
                return;

            auto current_size = get_current_file_size();
            while (log_str.size()) {
                auto first = log_str.begin();

                current_size += first->size();

                if (current_size >= max_file_size) {
                    _file->close();
                    _file = nullptr;
                    check_move_file();
                    _file = open_file();
                    if (!_file)
                        return;
                }

                if (!write_content(_file, *first))
                    return;

                log_str.erase(first);
            }
        }

        std::vector<std::string> get_queue(
        ) {
            std::lock_guard <std::recursive_mutex>_guard(mtx_access);
            std::vector<std::string> retval = log_queue;
            log_queue.clear();
            return retval;
        }

        bool wait_at_write(
        ) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return log_queue.size() > 0; });
            return true;
        }

        void init_thread(
        ) {
            auto bounded_function = std::bind(
                    &logger::writer_thread,
                    this
                );

            std::thread(std::bind(bounded_function)).detach();
        }

        void check_generate_path(
        ) {
            if (boost::filesystem::exists(this->file_path))
                return;

            boost::system::error_code _ec;
            boost::filesystem::create_directories(this->file_path, _ec);
        }

    private:
        std::string file_path;
        std::string file_name;

        bool is_writing;

        int32_t max_file_size;

        std::recursive_mutex mtx_access;
        std::mutex mtx;

        std::vector<std::string> log_queue;
        std::condition_variable cv;
    };
}
#endif // !LOGGER_H
