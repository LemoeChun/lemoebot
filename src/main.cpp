/* We simply call the root header file "App.h", giving you uWS::App and uWS::SSLApp */
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>

#include <uWebSockets/App.h>
#include <nlohmann/json.hpp>
#include <toml++/toml.hpp>

#include "command.h"
/* This is a simple WebSocket echo server example.
 * You may compile it with "WITH_OPENSSL=1 make" or with "make" */

using json = nlohmann::json;


int main() {
    /* ws->getUserData returns one of these */
    struct PerSocketData {
        /* Fill with user data */
    };

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App(
        /* There are example certificates in uWebSockets.js repo */
    ).ws<PerSocketData>("/*", {
        /* Settings */
        .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR | uWS::DEDICATED_DECOMPRESSOR),
        .maxPayloadLength = 100 * 1024 * 1024,
        .idleTimeout = 16,
        .maxBackpressure = 100 * 1024 * 1024,
        .closeOnBackpressureLimit = false,
        .resetIdleTimeoutOnSend = false,
        .sendPingsAutomatically = true,
        /* Handlers */
        .upgrade = nullptr,
        .open = [](auto *ws) {
            /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */

        },
        .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
            /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
             * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for 
             * benchmarking of large message sending without compression */

             /* Never mind, it changed back to never compressing for now */
            //std::cout << message << "\n";
            //ws->send(message, opCode, false);
            auto msg = json::parse(message);
            std::cout << msg.dump(4) << std::endl;
            if (msg["post_type"] == "message"){
                json resp_msg = ProcessCommand(msg);
                if (!resp_msg.empty()){
                    std::cout << resp_msg.dump(4) << std::endl;
                    ws->send(resp_msg.dump());
                }
            }
        },
        .dropped = [](auto */*ws*/, std::string_view /*message*/, uWS::OpCode /*opCode*/) {
            /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
        },
        .drain = [](auto */*ws*/) {
            /* Check ws->getBufferedAmount() here */
        },
        .ping = [](auto */*ws*/, std::string_view) {
            /* Not implemented yet */
        },
        .pong = [](auto */*ws*/, std::string_view) {
            /* Not implemented yet */
        },
        .close = [](auto */*ws*/, int /*code*/, std::string_view /*message*/) {
            /* You may access ws->getUserData() here */
        }
    }).listen(3100, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port " << 3100 << std::endl;
        }
    }).run();
}
