
# #include <iostream>


# class B
# {
#     public:
#         B() {std::cout << "say hi" << std::endl;};
#         B(const B &other) {std::cout << "say hi" << std::endl;};
#         ~B() {std::cout << "Destruct" << std::endl;};
# };

# #include <vector>


# int main()
# {
#     std::vector<B> allb;


#     B test;
#     allb.push_back(test);
#     allb.push_back(test);

#     std::cout << "---------\n" << std::endl;;
#     allb.erase(allb.begin());
#     allb.erase(allb.begin());
#     std::cout << "---------\n";
# }



import socket

IP = '127.0.0.1'
PORT = 8080

# Create a raw HTTP GET request
request = f"GET / HTTP/1.1\r\nHost: {IP}:{PORT}\r\nConnection: close\r\n\r\n"

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((IP, PORT))
    sock.sendall(request.encode())
    response = sock.recv(4096)
    print(response.decode())