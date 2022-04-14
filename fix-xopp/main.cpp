//
// Created by core on 13/04/2022.
//

#include <string_view>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>
#include <boost/date_time.hpp>
#include <pugixml.hpp>

namespace bp = boost::process;
namespace pt = boost::posix_time;
namespace fs = boost::filesystem;
namespace ba = boost::algorithm;

// https://stackoverflow.com/a/6417908
std::string_view remove_extension(const std::string_view& filename)
{
    size_t lastdot = filename.find_last_of('.');
    if (lastdot == std::string::npos) { return filename; }
    return filename.substr(0, lastdot);
}

std::string abs_to_rel(const fs::path dir, const std::string_view abs)
{
    fs::path p(abs.data());
    if (!fs::exists(p))
    {
        std::cerr << "file not found: " << abs << '\n';
        exit(1);
    }
    auto relp = fs::relative(p, dir);
    std::string rel(relp.string());
    if (!fs::exists(fs::path(dir.string() + "/" + relp.string())))
    {
        std::cerr << "conversion failed: " << abs << " ->> " << rel << '\n';
        exit(1);
    }
    return rel;
}

void traverse(const fs::path dir, pugi::xml_node& node)
{
    for (auto sub = node.first_child(); sub; sub = sub.next_sibling())
    {
        for (auto attr = sub.first_attribute(); attr; attr = attr.next_attribute())
        {
            if (std::string(sub.name()) == "background" && std::string(attr.name()) == "filename")
            {
                const std::string_view abs(attr.value());
                if (fs::exists(abs.data()))
                {
                    auto rel = abs_to_rel(dir, abs);
                    std::cout << attr.name() << ": " << abs << " --> " << rel << '\n';
                    attr.set_value(rel.data());
                }
                else
                {
                    std::cout << "file skipped: " << abs << '\n';
                }
            }
        }
        traverse(dir, sub);
    }
}

std::string get_time_date_stamp()
{
    auto now = pt::second_clock::local_time();
    auto date = now.date();
    auto time = now.time_of_day();
    std::stringstream stamp;
    stamp << std::fixed << std::setfill('0');
    stamp << date.year() << '-' << std::setw(2) << static_cast<int>(date.month()) << '-' << std::setw(2) << date.day()
          << '_' << std::setw(2) << time.hours() << '-' << std::setw(2) << time.minutes()
          << '-' << std::setw(2) << time.seconds();
    return stamp.str();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::vector<std::string> parts;
        boost::split(parts, argv[0], boost::is_any_of("/\\"));
        std::cout << "usage: " << parts.at(parts.size() - 1) << " <.xopp-file>\n";
        return 0;
    }
    const std::string_view filepath(argv[1]);

    if (filepath.ends_with("_backup.xopp"))
    {
        std::cout << "skipped xopp: " << filepath << '\n';
        ::exit(0);
    }

    if (!fs::exists(filepath.data()))
    {
        std::cerr << "file not found: " << filepath << '\n';
        ::exit(1);
    }

    pugi::xml_document doc;
    auto result = doc.load_file(filepath.data());
    if (!result)
    {
        std::cerr << "xml parsing failed\n";
        exit(1);
    }

    const auto dir = fs::path(filepath.data()).parent_path().string();
    const auto filename = fs::path(filepath.data()).filename().string();
    const auto filelabel = std::string(remove_extension(filename));

    traverse(dir, doc);

    const auto stamp = get_time_date_stamp();

    auto cache_dir = std::string(::getenv("HOME")) + "/.cache/fix-xopp";
    const auto tmp_path = cache_dir + "/" + stamp + "_" + filelabel + ".tmp";
    const auto backup_path = cache_dir + "/" + stamp + "_" + filelabel + ".xopp";

    fs::create_directories(cache_dir);

    if (doc.save_file(tmp_path.data()) != 1)
    {
        std::cerr << "could not save tmp xml file\n";
        return 1;
    }

    fs::rename(filepath.data(), backup_path.data());
    fs::rename(tmp_path.data(), filepath.data());
}