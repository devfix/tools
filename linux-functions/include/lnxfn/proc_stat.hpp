//
// Created by core on 08/04/2022.
//

#ifndef LNXFN_PROC_STAT_HPP
#define LNXFN_PROC_STAT_HPP

#include <cstdint>
#include <list>

struct proc_stat
{
    struct watch
    {
        explicit watch(std::size_t n);
        watch(const watch&) = delete;
        watch& operator=(const watch&) = delete;
        proc_stat update();
    private:
        const std::size_t n_;
        std::list<proc_stat> data_{};
    };

    proc_stat(const proc_stat&) = default;
    proc_stat& operator=(const proc_stat& ps) = default;
    proc_stat& operator+=(const proc_stat& ps);
    proc_stat& operator-=(const proc_stat& ps);
    friend proc_stat operator+(proc_stat lhs, const proc_stat& rhs); // passing lhs by value helps optimize chained a+b+c
    friend proc_stat operator-(proc_stat lhs, const proc_stat& rhs); // passing lhs by value helps optimize chained a+b+c
    static proc_stat get();
    [[nodiscard]] std::int32_t user() const;
    [[nodiscard]] std::int32_t nice() const;
    [[nodiscard]] std::int32_t system() const;
    [[nodiscard]] std::int32_t idle() const;

private:
    explicit proc_stat(std::int32_t user, std::int32_t nice, std::int32_t system, std::int32_t idle);

    std::int32_t user_;
    std::int32_t nice_;
    std::int32_t system_;
    std::int32_t idle_;
};

#endif //LNXFN_PROC_STAT_HPP
