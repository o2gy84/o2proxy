#include <pwd.h>
#include <iostream>

#include "server.hpp"
#include "logger.hpp"
#include "config.hpp"


std::string g_Version = "0.1.3";

std::string default_path()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);

    if (!pw)
    {
        return "";
    }
    std::string config = std::string(pw->pw_dir) + "/.o2proxy.conf";
    return config;
}

void init_config(const std::string &path, Config *conf)
{
    conf->add("port", "port to listen to", 7788);
    conf->add("engine", "input/output engine: select, poll or epoll", "epoll");
    conf->add("local_address", "local address", "");

    std::string real_path = path;
    if (path.empty())
    {
        real_path = default_path();
    }

    conf->load(real_path);
    logd(conf->dump());
    logd5(conf->usage());
}

int main(int argc, char *argv[])
{
    try
    {
        Options *opt = Options::impl();
        opt->add("help", "h", "print help and exit", false);
        opt->add("port", "p", "port to listen to (1025..65536)", 0);
        opt->add("loglevel", "l", "loglevel (1..5)", 0);
        opt->add("config", "c", "path to config", "");
        opt->add("syslog", "", "write logs into syslog", false);

        opt->parse(argc, argv);

        Logger::get().setOptionLogLevel(opt->get<int>("loglevel"));
        Logger::get().setOptionSyslog(argv[0], opt->get<bool>("syslog"));

        logd(opt->dump());
        if (opt->get<bool>("help"))
        {
            logi(opt->usage(argv[0]));
            return 0;
        }

        init_config(opt->get<std::string>("config"), Config::impl());


        logi("o2proxy version: {0}", g_Version);

        Server s(*opt);
        s.run();
        return 0;
    }
    catch (const std::exception &e)
    {
        kvloge("terminated: ", e.what());
    }
    return 0;
}
