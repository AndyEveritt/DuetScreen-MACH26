#ifndef UART_CONTROLLER_H
#define UART_CONTROLLER_H

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include <termios.h>

class UartController {
public:
    static constexpr size_t DEFAULT_BUFFER_SIZE = 4096;
    
    using DataCallback = std::function<void(const uint8_t*, size_t)>;

    UartController();
    ~UartController();

    // Connection
    bool open(const std::string& device);
    void close();
    bool isOpen() const { return fd >= 0; }

    // Configuration
    bool setBaudRate(speed_t baudRate);
    speed_t getBaudRate() const { return m_currentBaudRate; }
    bool setParameters(int dataBits, int stopBits, char parity);
    
    // Data handling
    bool send(const uint8_t* data, size_t length);
    bool send(const std::string& data) { return send((const uint8_t*)data.c_str(), data.length()); }
    void setReceiveCallback(DataCallback callback) { m_receiveCallback = callback; }
    
    // Buffer management
    void setBufferSize(size_t size);
    size_t getBufferSize() const { return m_bufferSize; }

private:
    int fd;
    std::atomic<bool> m_running;
    std::thread m_readThread;
    DataCallback m_receiveCallback;
    size_t m_bufferSize;
    speed_t m_currentBaudRate;
    
    void readLoop();
    bool configurePort();
};

#endif
