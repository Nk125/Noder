#include <Server/RequestSender.hpp>

std::string RequestSender::genRanStr(const size_t len) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> uid(0, 127);

    std::string buf;
    std::generate_n(std::inserter(buf, buf.end()), len, [&]() -> char
                    { return (char)(uid(mt)); });
    return buf;
}

#if SHOW_RPS
std::atomic<size_t> RequestSender::reqs = 0;

void RequestSender::logRequest() {
    reqs++;

    Time::timePoint now = Time::now();
    float secs = (Time::diff(Time::tp, now).count()) / 1000;

    if (std::fmod(secs, 3) == 0) {
        std::cout << "\rAvg RPS: " << abs(float(float(reqs) / float(secs)));
    }
}

void RequestSender::setupLogger() {
    reqs = 0;
    Time::tp = Time::now();
}
#endif

#if ENABLE_REGEX_IN_POST_REQUESTS
void RequestSender::regenBody(std::string &body, std::string originalBody) {
    body.clear();

    std::regex regRule(R"(\{random([0-9]+)\})", std::regex_constants::icase);
    std::smatch rm;

    while (std::regex_search(originalBody, rm, regRule)) {
        body.append(rm.prefix());

        size_t size = std::stoi(rm[1]);

        body.append(genRanStr(size));

        originalBody = rm.suffix();
    }

    body.append(originalBody);
}
#endif

template <bool RequestType>
void RequestSender::request(std::shared_ptr<httplib::Client> c, RequestSender::Options opt) {
    // httplib::Client c(url);

    // c.set_keep_alive(true);
    // c.set_follow_location(true);

    if constexpr (RequestType) {
        c->Post(std::get<1>(opt), std::get<2>(opt), std::get<3>(opt));
    }
    else {
        c->Get(std::get<3>(opt));
    }

#if SHOW_RPS
    logRequest();
#endif
}

template <bool RequestType>
RequestSender::Options RequestSender::parseOptions(const std::string& uri, urlparser& u, const nlohmann::json& config) {
    std::string body, ctype;
    
    if constexpr (RequestType) {
        body = config[std::to_string(Body)];
        ctype = config[std::to_string(ContentType)];

        u.body_ = body;
        u.ctype_ = ctype;
    }

    // Scheme
    std::string finalurl = (u.protocol_.empty() ? "http" : u.protocol_) + 
    // Host
    "://" + u.host_,
    // Path
    finalpath = (u.path_.empty() ? "/" : u.path_) +
    // Query
    (u.query_.empty() ? "" : "?" + u.query_);

    // Complete will be something like:
    // Scheme + Host         Path + Query
    // http://www.google.com /search?q=hello

    return std::make_tuple(finalurl, finalpath, body, ctype);
}

std::shared_ptr<httplib::Client> RequestSender::makeClient(const std::string& url, const httplib::Headers& h) {
    std::shared_ptr<httplib::Client> c = std::make_shared<httplib::Client>(url);
    c->set_keep_alive(true);
    c->set_follow_location(true);
    c->set_default_headers(h);
    return c;
}

void RequestSender::getHTTPRequest(const std::string& uri, const nlohmann::json& config) {
#if SHOW_RPS
    setupLogger();
#endif

    urlparser u(uri);

    if (u.host_.empty())
        return;

    httplib::Headers h{
        {"User-Agent", config[std::to_string(UserAgent)]}
    };

    Options opt = parseOptions<false>(uri, u, config);

    std::shared_ptr<httplib::Client> c = makeClient(std::get<0>(opt), h);

    while (!stopThreads) {
#if ASYNC_REQUEST_SEND
        std::future<void> fv = std::async(std::launch::async, RequestSender::request<false>, c, opt);
#else
        RequestSender::request<false>(c, opt);
#endif

#if SHOW_RPS
        logRequest();
#endif
    }
}

void RequestSender::postHTTPRequest(const std::string& uri, const nlohmann::json& config) {
#if SHOW_RPS
    setupLogger();
#endif

    urlparser u(uri);

    if (u.host_.empty())
        return;
    
    Options opt = parseOptions<true>(uri, u, config);

    httplib::Headers h{
        {"User-Agent", config[std::to_string(UserAgent)]}
    };

#if ENABLE_REGEX_IN_POST_REQUESTS
    bool useRgx = config[std::to_string(UseRegex)];

    std::string body = std::get<2>(opt);
#endif

    std::shared_ptr<httplib::Client> c = makeClient(std::get<0>(opt), h);

    while (!stopThreads) {
#if ENABLE_REGEX_IN_POST_REQUESTS
        if (useRgx) {
            regenBody(body, std::get<2>(opt));
        }
#endif

#if ASYNC_REQUEST_SEND
        std::future<void> fv = std::async(std::launch::async, RequestSender::request<true>, c, opt);
#else
        RequestSender::request<true>(c, opt);
#endif
    }
}

std::atomic<bool> RequestSender::stopThreads = false;
