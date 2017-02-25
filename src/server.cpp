#include "server.hpp"
#include "poll.hpp"
#include "poll_async.hpp"
#include "epoll.hpp"
#include "select.hpp"
#include "logger.hpp"
#include "config.hpp"

namespace 
{

enum class engine_t: uint8_t
{
    UNKNOWN,
    SELECT,
    POLL,
    EPOLL
};

engine_t string2engine(const std::string &str)
{
    if (str == "select") return engine_t::SELECT;
    if (str == "poll") return engine_t::POLL;
    if (str == "epoll") return engine_t::EPOLL;
    return engine_t::UNKNOWN;
}

std::string engine2string(engine_t engine)
{
    if (engine == engine_t::SELECT) return "select";
    if (engine == engine_t::POLL)   return "poll";
    if (engine == engine_t::EPOLL)  return "epoll";
    return "unknown";
}

std::unique_ptr<Engine> get_engine(engine_t type, int port)
{
    std::unique_ptr<Engine> ret;
    switch (type)
    {
        case engine_t::SELECT:
        {
            ret.reset(new SelectEngine(port));
            break;
        }
        case engine_t::POLL:
        {
            //if (async) ret.reset(new AsyncPollEngine(port));
            if (1)      ret.reset(new AsyncPollEngine(port));
            else        ret.reset(new PollEngine(port));
            break;
        }
        case engine_t::EPOLL:
        {
            //if (async) ret.reset(new EpollEngine(port));
            if (1)     ret.reset(new EpollEngine(port));
            else       ret.reset(new EpollEngine(port)); 
            break;
        }
        default: throw std::logic_error("unknown engine type");
    }

    kvlogi("use engine: ", engine2string(type));
    return ret;
}


} // namespace

Server::Server(const Options &opt)
{
    int port = Config::impl()->get<int>("port");

    int opt_port = opt.get<int>("port");
    if (opt_port)
    {
        // opt overrides
        port = opt_port;
    }

    engine_t engine_type = engine_t::UNKNOWN;
    engine_type = string2engine(Config::impl()->get<std::string>("engine"));
    m_Engine = get_engine(engine_type, port);
}

void Server::run()
{
    m_Engine->run();
}
