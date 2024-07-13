#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>

namespace fsv {
	using filter = std::function<bool(const char&)>;
	class filtered_string_view {
		class iter {
			friend class filtered_string_view;

		 public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using reference_type = const char&;
			using pointer_type = void;
			using difference_type = std::ptrdiff_t;
			// constructor
			iter(const char* pc, filter pred) noexcept;
			// get iter pointer
			auto operator*() const noexcept -> reference_type;
			auto operator->() const noexcept -> pointer_type;
			// iter ++ --
			auto operator++() noexcept -> iter&;
			auto operator++(int) noexcept -> iter;
			auto operator--() noexcept -> iter&;
			auto operator--(int) noexcept -> iter;
			// compare iter
			friend auto operator==(const iter& lhs, const iter& rhs) noexcept -> bool;
			friend auto operator!=(const iter& lhs, const iter& rhs) noexcept -> bool;

		 private:
			using pointer = const char*;
			pointer pc_;
			filter pred_;
		};

	 public:
		// iterator
		using const_iterator = iter;
		using iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		// begin end
		auto begin() const noexcept -> iterator;
		auto cbegin() const noexcept -> const_iterator;
		auto end() const noexcept -> iterator;
		auto cend() const noexcept -> const_iterator;
		// reverse begin end
		auto rbegin() const noexcept -> reverse_iterator;
		auto crbegin() const noexcept -> const_reverse_iterator;
		auto rend() const noexcept -> reverse_iterator;
		auto crend() const noexcept -> const_reverse_iterator;

		// fsv
		static filter default_predicate;
		// constructor
		filtered_string_view() noexcept;
		filtered_string_view(const std::string& s) noexcept;
		filtered_string_view(const std::string& s, filter predicate) noexcept;
		filtered_string_view(const char* s) noexcept;
		filtered_string_view(const char* s, filter predicate) noexcept;
		// copy and move
		filtered_string_view(const filtered_string_view& other) noexcept;
		filtered_string_view(filtered_string_view&& other) noexcept;

		auto operator=(const filtered_string_view& other) noexcept -> filtered_string_view&;
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&;
		auto operator[](int n) const -> const char&;

		// type conversion by filtered
		explicit operator std::string() const;

		// get data originally
		auto data() const noexcept -> const char*;
		// get predicate function
		auto predicate() const noexcept -> const filter&;
		// get a character after filtered
		auto at(int index) const -> const char&;
		// get size after filtered
		auto size() const -> std::size_t;
		// check empty after filtered
		auto empty() const noexcept -> bool;

		// Destructor
		~filtered_string_view() = default;

	 private:
		using pointer = const char*;
		pointer data_;
		std::size_t size_;
		filter pred_;
	};

	// get data after many filtered function
	auto
	compose(const filtered_string_view& filtered_sv, const std::vector<filter>& filts) noexcept -> filtered_string_view;
	// get split data after filtered
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view>;
	// get sub-data after filtered
	auto substr(const filtered_string_view& fsv, int pos = 0, int count = 0) -> filtered_string_view;

	// compare two fsv
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;
	auto operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;
	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering;
	// output fsv
	auto operator<<(std::ostream& os, const filtered_string_view& filtered_sv) noexcept -> std::ostream&;
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
