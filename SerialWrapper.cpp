#include "SerialWrapper.h"

SerialWrapper::SerialWrapper()
    :io(), serial(io)
{
}

SerialWrapper::SerialWrapper(std::string port, unsigned int baud_rate)
    : io(), serial(io, port)//, timeout(io)
{
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
}

void SerialWrapper::writeString(std::string s)
{
    boost::asio::write(serial, boost::asio::buffer(s.c_str(), s.size()));
}

void SerialWrapper::asyncReadLine(std::function<void(const std::string&)> callBack)
{
    readCallback = callBack;
    boost::asio::async_read_until(
        serial, 
        buf, 
        "\n", 
        [this](const boost::system::error_code& ec, std::size_t bytesReceived) 
        { 
            this->lineReceived(ec, bytesReceived);
        });
    
}

void SerialWrapper::lineReceived(const boost::system::error_code& ec, std::size_t size)
{
    if (ec) return;

    std::istream str(&buf);
    std::string line;
    std::getline(str, line);

    if (readCallback) {
       readCallback(line);
    }
}

bool SerialWrapper::isOpen() const
{
    return serial.is_open();
}

void SerialWrapper::open(std::string port, unsigned int baud_rate)
{
    if (!serial.is_open()) {
        serial.open(port);

        serial.set_option(boost::asio::serial_port_base::baud_rate(9600));
    }


}



void SerialWrapper::close()
{
    serial.close();
}

SerialWrapper::~SerialWrapper() {
    if (serial.is_open()) {
        serial.close();
    }

}