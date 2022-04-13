//
// Created by core on 07/03/2022.
//

#include <string_view>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>
#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>
#include <pugixml.hpp>

namespace bp = boost::process;
namespace pt = boost::posix_time;
namespace fs = boost::filesystem;
namespace ba = boost::algorithm;

// https://stackoverflow.com/a/6417908
std::string_view remove_extension(const std::string_view &filename) {
    size_t lastdot = filename.find_last_of('.');
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}

std::string abs_to_rel(const std::string_view abs) {
    fs::path p(abs.data());
    if (!fs::exists(p)) {
        std::cerr << "file not found: " << abs << '\n';
        exit(1);
    }
    auto dir = fs::current_path();
    auto relp = fs::relative(p, dir);
    std::string rel(relp.string());
    if (!fs::exists(relp)) {
        std::cerr << "conversion failed: " << abs << " ->> " << rel << '\n';
        exit(1);
    }
    return rel;
}

void traverse(pugi::xml_node &node) {
    for (auto sub = node.first_child(); sub; sub = sub.next_sibling()) {
        for (auto attr = sub.first_attribute(); attr; attr = attr.next_attribute()) {
            if (std::string(attr.name()) == "src") {
                const std::string_view abs(attr.value());
                if (fs::exists(abs.data())) {
                    auto rel = abs_to_rel(abs);
                    std::cout << attr.name() << ": " << abs << " --> " << rel << '\n';
                    attr.set_value(rel.data());
                } else {
                    std::cout << "file skipped: " << abs << '\n';
                }
            }
        }
        traverse(sub);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::vector<std::string> parts;
        boost::split(parts, argv[0], boost::is_any_of("/\\"));
        std::cout << "usage: " << parts.at(parts.size() - 1) << " <.mmpz-file>\n";
        return 0;
    }
    const std::string_view filepath(argv[1]);
    const fs::path inputpath(filepath.data());

    if(!fs::exists(inputpath))
    {
        std::cerr << "file not found: " << filepath << '\n';
        exit(1);
    }

    pugi::xml_document doc;
    pugi::xml_parse_result result;

    std::string lower(filepath);
    ba::to_lower(lower);
    if (ba::ends_with(lower, ".mmpz")) {
        std::vector<std::string> args{"-d", filepath.data()};
        bp::ipstream out;
        bp::child c(bp::search_path("lmms"), args, bp::std_out > out);
        result = doc.load(out);
        c.wait();
    } else {
        result = doc.load_file(filepath.data());
    }

    if (!result) {
        std::cerr << "xml parsing failed\n";
        exit(1);
    }

    fs::path dir = fs::path(inputpath).parent_path();
    fs::current_path(dir);
    const auto fn_name = inputpath.filename();
    const auto filename = remove_extension(fn_name.string());

    traverse(doc);

    fs::path tmppath(std::string(filename) + ".tmp");
    fs::path outputpath(std::string(filename) + ".mmp");

    auto tmp_str = tmppath.string();
    std::cout << '\n';
    std::cout << "Saving result: " << doc.save_file(tmp_str.data()) << std::endl;

    auto now = pt::second_clock::local_time();
    auto date = now.date();
    auto time = now.time_of_day();
    std::stringstream stamp;
    stamp << std::fixed << std::setfill('0');
    stamp << date.year() << '-' << std::setw(2) << static_cast<int>(date.month()) << '-' << std::setw(2) << date.day()
          << '_' << std::setw(2) << time.hours() << '-' << std::setw(2) << time.minutes()
          << '-' << std::setw(2) << time.seconds();

    fs::path backuppath(std::string(filename) + '_' + stamp.str() + "_backup.mmpx");
    fs::rename(inputpath, backuppath);

    fs::rename(tmppath, outputpath);
}