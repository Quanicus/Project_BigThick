#include <uWebSockets/App.h>
#include <iostream>
#include <string>

struct UserData { 
	std::string name;
};

int main() {
    uWS::App()
        .get("/*", [](auto *res, auto *req) {
            res->end("Hello, this is a simple WebSocket server!");
        })
        .ws<UserData>("/*", {
            .open = [](auto *ws) {
                std::cout << "A new WebSocket connection has opened!" << std::endl;
                ws->send("Welcome to the WebSocket server!");
            },
            .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                std::cout << "Received message: " << message << std::endl;
                ws->send("You said: " + std::string(message));
            },
            .close = [](auto *ws, int /*code*/, std::string_view /*message*/) {
                std::cout << "A WebSocket connection was closed!" << std::endl;
            }
        })
        .listen(9001, [](auto *token) {
            if (token) {
                std::cout << "Server started on port 9001!" << std::endl;
            } else {
                std::cout << "Failed to start server!" << std::endl;
            }
        })
        .run();
}
