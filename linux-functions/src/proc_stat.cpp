//
// Created by core on 08/04/2022.
//

#include <lnxfn/proc_stat.hpp>
#include <fstream>
#include <torquis.hpp>

proc_stat::watch::watch(std::size_t n) : n_(n) { data_.push_back(get()); }

proc_stat proc_stat::watch::update()
{
    data_.push_back(get());
    if (data_.size() > n_ + 1) { data_.pop_front(); }
    return data_.back() - data_.front();
}

proc_stat& proc_stat::operator+=(const proc_stat& ps)
{
    user_ += ps.user_;
    nice_ += ps.nice_;
    system_ += ps.system_;
    idle_ += ps.idle_;
    return *this;
}

proc_stat& proc_stat::operator-=(const proc_stat& ps)
{
    user_ -= ps.user_;
    nice_ -= ps.nice_;
    system_ -= ps.system_;
    idle_ -= ps.idle_;
    return *this;
}

proc_stat proc_stat::get()
{
    std::string load_line;
    {
        std::ifstream ifs("/proc/stat");
        std::getline(ifs, load_line);
    }
    auto params = torquis::split(load_line, " ");

    std::size_t k = 0;
    while (params.at(++k).empty()) {} // skip empty string params

    return proc_stat{
        static_cast<std::int32_t>(std::stol(std::string(params.at(k + 0)))),
        static_cast<std::int32_t>(std::stol(std::string(params.at(k + 1)))),
        static_cast<std::int32_t>(std::stol(std::string(params.at(k + 2)))),
        static_cast<std::int32_t>(std::stol(std::string(params.at(k + 3))))
    };
}

std::int32_t proc_stat::user() const { return user_; }

std::int32_t proc_stat::nice() const { return nice_; }

std::int32_t proc_stat::system() const { return system_; }

std::int32_t proc_stat::idle() const { return idle_; }

proc_stat operator+(proc_stat lhs, const proc_stat& rhs)
{
    return proc_stat{
        lhs.user_ + rhs.user_,
        lhs.nice_ + rhs.nice_,
        lhs.system_ + rhs.system_,
        lhs.idle_ + rhs.idle_
    };
}

proc_stat operator-(proc_stat lhs, const proc_stat& rhs)
{
    return proc_stat{
        lhs.user_ - rhs.user_,
        lhs.nice_ - rhs.nice_,
        lhs.system_ - rhs.system_,
        lhs.idle_ - rhs.idle_
    };
}

proc_stat::proc_stat(std::int32_t user, std::int32_t nice, std::int32_t system, std::int32_t idle) :
    user_(user), nice_(nice), system_(system), idle_(idle) {}
