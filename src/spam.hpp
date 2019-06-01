#include <filesystem>
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
	pattern(std::string bits);

	auto reduce(size_t k) const
		-> pattern;

	auto begin() const
	{
		return indices.begin();
	}

	auto end() const
	{
		return indices.end();
	}

	auto size() const
		-> size_t
	{
		return bits.size();
	}

	auto weight() const
		-> size_t
	{
		return indices.size();
	}

	auto operator[](size_t n) const
		-> size_t
	{
		return indices[n];
	}

	auto operator==(pattern const& rhs) const
		-> bool
	{
		return indices == rhs.indices;
	}
};

struct sequence {
	std::string name;
	std::string bases;

	static auto from_directory(std::filesystem::path const& path)
		-> std::optional<std::vector<sequence>>;

	static auto from_multi_fasta_file(std::filesystem::path const& filename)
		-> std::optional<std::vector<sequence>>;

	auto size() const
		-> size_t
	{
		return bases.size();
	}

	auto begin() const
	{
		return bases.begin();
	}

	auto end() const
	{
		return bases.end();
	}
};

auto operator>>(std::istream& is, sequence& seq)
	-> std::istream&;

using word_t = uint64_t;

class wordlist {
	std::vector<word_t> words;
	spam::pattern pattern;

public:
	static auto max_wordsize()
		-> size_t;

public:
	wordlist(
		spam::sequence const& sequence,
		spam::pattern pattern);

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

	std::vector<wordlist> wordlists;
	std::vector<std::vector<double>> matrix;

	std::shared_ptr<ThreadPool> threadpool;

public:
	distance_matrix(
		std::vector<spam::sequence> sequences,
		spam::pattern pattern,
		std::shared_ptr<ThreadPool> threadpool =
			std::make_shared<ThreadPool>(std::thread::hardware_concurrency()));

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
};

std::ostream& operator<<(std::ostream& os, distance_matrix const& matrix);

} // namespace spam
