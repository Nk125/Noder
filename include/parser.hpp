#pragma once
#include <algorithm>
#include <cctype>
#include <functional>
#include <string>

class urlparser {
public:
    urlparser(const std::string &url_s) {
        this->parse(url_s);
    }

    std::string protocol_, host_, path_, query_, ctype_, body_;

private:
    void parse(const std::string &url_s);
};
// ctors, copy, equality, ...

void urlparser::parse(const std::string& url_s) {
    const std::string prot_end("://");

    std::string::const_iterator prot_i = std::search(url_s.begin(),
        url_s.end(),
        prot_end.begin(),
        prot_end.end()
    );

    protocol_.reserve(std::distance(url_s.begin(), prot_i));

    std::transform(url_s.begin(), prot_i,
        std::back_inserter(protocol_),
        [](char c) -> char {return tolower(c); }); // protocol is icase

    if (prot_i == url_s.end())
        return;

    std::advance(prot_i, prot_end.length());

    std::string::const_iterator path_i = std::find(prot_i, url_s.end(), '/');
    host_.reserve(std::distance(prot_i, path_i));

    std::transform(prot_i, path_i,
        std::back_inserter(host_),
        [](char c) -> char {return tolower(c); }); // host is icase

    std::string::const_iterator query_i = std::find(path_i, url_s.end(), '?');

    path_.assign(path_i, query_i);

    if (query_i != url_s.end())
        ++query_i;

    query_.assign(query_i, url_s.end());
}