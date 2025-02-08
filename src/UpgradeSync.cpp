/* We simply call the root header file "App.h", giving you uWS::App and uWS::SSLApp */
#include <uWebSockets/App.h>
#include <filesystem>
#include <string>
#include <unordered_map>

/* This is a simple WebSocket "sync" upgrade example.
 * You may compile it with "WITH_OPENSSL=1 make" or with "make" */

std::string listDirectories() {
    const std::string path = "/mnt/ssd/clusterVolume1/media/movies/hls";  // Hardcoded static path
    std::string result = "[";
    bool first = true;

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            if (!first) {
                result += ", ";
            }
            result += "\"" + entry.path().filename().string() + "\"";
            first = false;
        }
    }

    result += "]";
    return result;
}

int main() {
    /* ws->getUserData returns one of these */
    struct PerSocketData {
        /* Define your user data */
        int id;
    };
	int count {0};
	std::string nowPlaying = "bustin";
	
	uWS::App *app = new uWS::App();
    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
	app->get("/uServer/nowPlaying", [&nowPlaying](auto *res, auto *req) {
		(void) req;
		res->writeHeader("Content-Type", "text/plain");
		res->end(nowPlaying);
	})
	.get("/uServer/movielist", [](auto *res, auto *req) {
        (void) req;
		std::string dirList = listDirectories();
        res->writeHeader("Content-Type", "application/json");
        res->end(dirList);
    })	
	.ws<PerSocketData>("/ws", {
        /* Settings */
        .compression = uWS::SHARED_COMPRESSOR,
        .maxPayloadLength = 16 * 1024,
        .idleTimeout = 10,
        .maxBackpressure = 1 * 1024 * 1024,
        /* Handlers */
        .upgrade = [&count](auto *res, auto *req, auto *context) {

            /* You may read from req only here, and COPY whatever you need into your PerSocketData.
             * PerSocketData is valid from .open to .close event, accessed with ws->getUserData().
             * HttpRequest (req) is ONLY valid in this very callback, so any data you will need later
             * has to be COPIED into PerSocketData here. */

            /* Immediately upgrading without doing anything "async" before, is simple */
            res->template upgrade<PerSocketData>({
                /* We initialize PerSocketData struct here */
                .id = count++
            }, req->getHeader("sec-websocket-key"),
                req->getHeader("sec-websocket-protocol"),
                req->getHeader("sec-websocket-extensions"),
                context);

            /* If you don't want to upgrade you can instead respond with custom HTTP here,
             * such as res->writeStatus(...)->writeHeader(...)->end(...); or similar.*/

            /* Performing async upgrade, such as checking with a database is a little more complex;
             * see UpgradeAsync example instead. */
        },
        .open = [](auto *ws) {
            /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct.
             * Here we simply validate that indeed, something == 13 as set in upgrade handler. */
            //std::cout << "Something is: " << static_cast<PerSocketData *>(ws->getUserData())->something << std::endl;
			ws->subscribe("videoSync");
        },
        .message = [&app, &nowPlaying](auto *ws, std::string_view message, uWS::OpCode opCode) {
			(void) ws;
			//(void) opCode;
			/* We simply echo whatever data we get */
            //ws->send(message, opCode);
			//std::cout << message << std::endl;
			if (message.find(R"("type":"load")") != std::string_view::npos) {
				size_t start_pos = message.find(R"("movie":")") + 9;
				size_t end_pos = message.find('\"', start_pos);
				nowPlaying = message.substr(start_pos, end_pos - start_pos);
				std::cout << nowPlaying << std::endl;
				app->publish("videoSync", message, opCode);
			} else {
				ws->publish("videoSync", message, opCode);
			}
        },
        .drain = [](auto */*ws*/) {
            /* Check ws->getBufferedAmount() here */
        },
        .ping = [](auto */*ws*/, std::string_view) {
            /* You don't need to handle this one, we automatically respond to pings as per standard */
        },
        .pong = [](auto */*ws*/, std::string_view) {
            /* You don't need to handle this one either */
        },
        .close = [](auto */*ws*/, int /*code*/, std::string_view /*message*/) {
            /* You may access ws->getUserData() here, but sending or
             * doing any kind of I/O with the socket is not valid. */
        }
    }).listen(9001, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port " << 9001 << std::endl;
        }
    });

	app->run();
	delete app;
	
	uWS::Loop::get()->free();	
}
