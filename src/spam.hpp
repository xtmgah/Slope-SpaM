#include <iosfwd>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <threadpool/ThreadPool.h>

namespace spam {

class pattern {
	std::string bits;
	std::vector<size_t> indices;

public:
	pattern(std::string _bits)
		: bits(std::move(_bits))
	{
		for (size_t i = 0; i < bits.size(); ++i) {
			if (bits[i] == '1') {
				indices.push_back(i);
			}
		}
	}

	auto begin() const {
		return indices.begin();
	}

	auto end() const {
		return indices.end();
	}

	size_t weight() const {
		return indices.size();
	}

	auto operator[](size_t n) const
		-> size_t
	{
		return indices[n];
	}
};

struct sequence {
	std::string name;
	std::string bases;

	static auto from_multi_fasta_file(std::string const& filename)
		-> std::optional<std::vector<sequence>>;
};

auto operator>>(std::istream& is, sequence& seq)
	-> std::istream&;

using word_t = uint64_t;

class wordlist {
	std::vector<word_t> words;

public:
	wordlist(
		spam::pattern const& pattern,
		spam::sequence const& sequence,
		size_t k);

	auto begin() const
	{
		return words.begin();
	}

	auto end() const
	{
		return words.end();
	}
};

class distance_matrix {
	std::vector<spam::sequence> sequences;
	spam::pattern pattern;
	size_t k;

	std::vector<wordlist> wordlists;
	std::vector<std::vector<double>> matrix;

	ThreadPool threadpool;

public:
	distance_matrix(
		std::vector<spam::sequence> const& sequences,
		spam::pattern const& pattern);

	distance_matrix(
		std::vector<spam::sequence>&& sequences,
		spam::pattern const& pattern);

	auto size() const
		-> size_t;

	auto column(size_t i) const
		-> std::pair<spam::sequence const&, std::vector<double> const&>;

private:
	void calculate();

	void initialize_matrix();
	void create_wordlists();
	void calculate_matrix();

	auto calculate_element(size_t i, size_t j) const
		-> std::pair<double, double>;
	static auto calculate_matches(
		spam::wordlist const& wordlist1,
		spam::wordlist const& wordlist2)
		-> size_t;
	auto calculate_distance(
		size_t matches,
		size_t length1,
		size_t length2) const
		-> std::pair<double, double>;
};

std::ostream& operator<<(std::ostream& os, distance_matrix const& matrix);

} // namespace spam
