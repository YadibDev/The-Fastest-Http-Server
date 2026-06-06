#include <iostream>
#include <string>
#include <cstring>      // من أجل strlen و memset
#include <sys/socket.h> // من أجل socket, connect, send, recv
#include <netinet/in.h> // من أجل sockaddr_in
#include <arpa/inet.h>  // من أجل inet_addr
#include <unistd.h>     // من أجل close

int main() {
    std::string ip_input;
    int port_input;

    // 1. استقبال الـ IP والـ Port من المستخدم
     ip_input= "127.0.0.1";
    port_input = 8082;

    // تنظيف الـ Buffer لتفادي تخطي دالة getline القادمة بسبب حرف '\n' المتبقي
    // 2. إنشاء الـ Socket (IPv4, TCP)
      int network_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (network_socket < 0) {
            std::cerr << "Error: Could not create socket." << std::endl;
            return 1;
        }
    
        // 3. إعداد عنوان السيرفر
        struct sockaddr_in server_address;
        std::memset(&server_address, 0, sizeof(server_address));
        
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port_input);
        server_address.sin_addr.s_addr = inet_addr(ip_input.c_str());
    
        if (server_address.sin_addr.s_addr == INADDR_NONE) {
            std::cerr << "Error: Invalid IP address." << std::endl;
            close(network_socket);
            return 1;
        }
    
        // 4. الاتصال بالسيرفر
        std::cout << "Connecting to " << ip_input << ":" << port_input << "..." << std::endl;

        if (connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        {
            std::cerr << "Error: Connection failed." << std::endl;
            close(network_socket);
            return 1;
        }
        std::cout << "Connected successfully!\n" << std::endl;
    
        // 5. استقبال الـ Request يدوياً من المستخدم
        std::string custom_request = "";
        std::string line;
    
        std::cout << "--- Enter your Request (Press ENTER on an empty line to SEND) ---" << std::endl;
        custom_request +="GET /cgi-bin/upload.php HTTP/1.1\r\n";
        custom_request +="Connection: keep-alive \r\n";
        custom_request +="Host: localhost:8082 \r\n";
        custom_request +="Cookie:1\r\n";
        custom_request +="Cookie: 2\r\n";
        custom_request +="Cookie: 3\r\n\r\n";
        // 6. إرسال الـ Request المدخل
        std::cout << "\nSending request..." << std::endl;
        if (send(network_socket, custom_request.c_str(), custom_request.length(), 0) < 0) {
            std::cerr << "Error: Failed to send request." << std::endl;
            close(network_socket);
            return 1;
        }
    
        // 7. استقبال الـ Response وطباعته كاملاً في الـ Output
        char response_buffer[4096];
        ssize_t bytes_received;

        // الـ Loop تستمر في القراءة حتى يغلق السيرفر الاتصال (تنتهي البيانات)
        while ((bytes_received = recv(network_socket, response_buffer, sizeof(response_buffer) - 1, 0)) > 0) {
            response_buffer[bytes_received] = '\0'; // ضمان وجود Null-terminator لطباعة النص بأمان
            std::cout << response_buffer;           // طباعة الجزء المستلم مباشرة
        }
    
        if (bytes_received < 0) {
            std::cerr << "\nError: Failed to receive data." << std::endl;
        }

        std::cout << "\nend" << std::endl;
    
        // 8. غلق الـ Socket
        close(network_socket);
    return 0;
}