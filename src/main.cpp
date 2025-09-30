#include "client_app.hpp"

#include <iostream>

int main(int argc, char **argv) {
    sotc::LaunchOptions options{};
    if (argc > 1) {
        options.server_host = argv[1];
    }
    if (argc > 2) {
        options.player_name = argv[2];
    }

    sotc::ClientApp app;
    app.configure(options);
    app.run();
    return 0;
}
