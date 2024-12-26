// test code that makes one thread handles mid-program key inputs as signals, might want to use this to print out current
// state of the server
#include <iostream>
#include <csignal>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using namespace std;
using namespace boost::asio;

void signal_handler(int signum) {
    cout << "here' help\n";
}

void handle_input(const boost::system::error_code& error, size_t bytes_transferred, char* input, boost::asio::posix::stream_descriptor& input_stream) {
    if (!error) {
        if (input[0] == 'h' || input[0] == 'H') {
            signal_handler(SIGUSR1);
        }
        // Start another asynchronous read
        async_read(input_stream, buffer(input, 1), boost::bind(handle_input, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, input, boost::ref(input_stream)));
    }
}

int main() {
    boost::asio::io_context io_context;
    boost::asio::posix::stream_descriptor input_stream(io_context, ::dup(STDIN_FILENO));
    char input[1];

    // Start asynchronous read
    async_read(input_stream, buffer(input, 1), boost::bind(handle_input, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, input, boost::ref(input_stream)));

    // Run the io_context to process asynchronous events
    io_context.run();

    return 0;
}
