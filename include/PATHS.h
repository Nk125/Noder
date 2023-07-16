#pragma once
#define STR constexpr const char*

namespace URL {
    STR GET = "/get";
    STR POST = "/post";
    
    namespace SERVER {
        STR CHECK = "/server/check";
        STR IP = "/server/ip";
        STR UPTIME = "/server/uptime";
        STR RESTART = "/server/restart";
        STR KILL = "/server/kill";

        namespace CONFIG {
            STR RELOAD = "/server/config/reload";
        }
    }
}

#undef STR