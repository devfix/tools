#include <iostream>
#include <boost/program_options.hpp>
#include <numeric>
#include <random>
#include <chrono>

#define KEY_MODE_SPECIAL_CHARS "#_&%$()?*+~|"
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	// number of characters to be generated
	int char_count;

	static po::options_description desc("Allowed options");
	desc.add_options()
			("help,h", "produce help message")
			(",l", "allow lower case letters")
			(",u", "allow upper case letters")
			(",d", "allow digits")
			("chars,c", po::value<std::vector<std::string>>(), "add additional characters")
			("key,k", "key mode, equivalent to -y -l -u -d -c '" KEY_MODE_SPECIAL_CHARS "'")
			("unify,y", "remove duplicate characters")
			("alphabet,a", "print created alphabet")
			(",n", po::value<int>(&char_count)->default_value(0), "set the count of generated characters");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	std::string alphabet;

	auto addAsciiSequence = [&](char begin, std::size_t n)
	{
		alphabet.resize(alphabet.size() + n);
		auto bwit = alphabet.rbegin();
		std::advance(bwit, n);
		std::iota(alphabet.rbegin(), bwit, begin);
	};

	bool key_mode = vm.count("key");
	if (key_mode) { alphabet += KEY_MODE_SPECIAL_CHARS; }

	if (key_mode || vm.count("-l")) { addAsciiSequence('a', 26); }
	if (key_mode || vm.count("-u")) { addAsciiSequence('A', 26); }
	if (key_mode || vm.count("-d")) { addAsciiSequence('0', 10); }

	if (vm.count("chars"))
	{
		const auto& vec = vm["chars"].as<std::vector<std::string>>();
		std::for_each(vec.begin(), vec.end(), [&](const auto& chars) { alphabet += chars; });
	}

	// sort the alphabet, so each combinated of '-c ...' leads to the same alphabet
	std::sort(alphabet.begin(), alphabet.end());

	// remove all except the first character from every consecutive group of same characters
	if (key_mode || vm.count("unify")) { alphabet.erase(std::unique(alphabet.begin(), alphabet.end()), alphabet.end()); }

	// print alphabet if desired
	if (vm.count("alphabet")) { std::cout << "alphabet: " << alphabet << '\n'; }
	else if (vm.count("help") || char_count <= 0)
	{
		std::cout << desc << '\n';
		std::cout << "Compiled at " << __DATE__ << " " << __TIME__ << '\n';
		return 1;
	}

	// abort if alphabet is empty
	if (alphabet.empty()) { throw std::runtime_error("empty alphabet, no characters supplied"); }

	// create pseudo random number generator with uniform distribution for each alphabet character
	const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine engine(seed);
	std::uniform_int_distribution<int> distribution(0, alphabet.length() - 1);
	auto gen = std::bind(distribution, engine);

	// create the random output string and print it
	std::string output(char_count, '\0');
	std::for_each(output.begin(), output.end(), [&](auto& c) { c = alphabet.at(gen()); });
	std::cout << output << std::endl;

	return 0;
}
