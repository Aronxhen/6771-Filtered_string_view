#include "./filtered_string_view.h"
#include <compare>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <utility>

namespace fsv {
	// class filtered_string_view::iter
	// Constructor:
	filtered_string_view::iter::iter(const char* pc, filter pred) noexcept
	: pc_(pc)
	, pred_(std::move(pred)) {}

	auto filtered_string_view::iter::operator*() const noexcept -> reference_type {
		return *pc_;
	}
	auto filtered_string_view::iter::operator->() const noexcept -> pointer_type {}
	// ++iter
	auto filtered_string_view::iter::operator++() noexcept -> iter& {
		pc_++;
		while (*pc_ != '\0' && !pred_(*pc_)) {
			pc_++;
		}
		return *this;
	}
	// iter++
	auto filtered_string_view::iter::operator++(int) noexcept -> iter {
		auto new_iter = *this;
		++(*this);
		return new_iter;
	}
	// --iter
	auto filtered_string_view::iter::operator--() noexcept -> iter& {
		pc_--;
		while (*pc_ != '\0' && !pred_(*pc_)) {
			pc_--;
		}
		return *this;
	}
	// iter--
	auto filtered_string_view::iter::operator--(int) noexcept -> iter {
		auto new_iter = *this;
		--(*this);
		return new_iter;
	}
	// == !=
	auto
	operator==(const filtered_string_view::iterator& lhs, const filtered_string_view::iterator& rhs) noexcept -> bool {
		return lhs.pc_ == rhs.pc_;
	}
	auto
	operator!=(const filtered_string_view::iterator& lhs, const filtered_string_view::iterator& rhs) noexcept -> bool {
		return lhs.pc_ != rhs.pc_;
	}

	// class filtered_string_view
	// iterator of filtered_string_view
	// begin
	auto filtered_string_view::begin() const noexcept -> iterator {
		const char* pc = data_;
		while (*pc != '\0' && !pred_(*pc)) {
			pc++;
		}
		return {pc, pred_};
	}
	auto filtered_string_view::cbegin() const noexcept -> const_iterator {
		return begin();
	}
	// end
	auto filtered_string_view::end() const noexcept -> iterator {
		return {data_ + size_, pred_};
	}
	auto filtered_string_view::cend() const noexcept -> const_iterator {
		return end();
	}
	// reverse
	auto filtered_string_view::rbegin() const noexcept -> reverse_iterator {
		return reverse_iterator(end());
	}
	auto filtered_string_view::crbegin() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator(cend());
	}
	auto filtered_string_view::rend() const noexcept -> reverse_iterator {
		return reverse_iterator(begin());
	}
	auto filtered_string_view::crend() const noexcept -> const_reverse_iterator {
		return reverse_iterator(cbegin());
	}

	// filtered_string_view
	filter filtered_string_view::default_predicate = [](const char&) { return true; };
	// Constructor:
	// default constructor
	filtered_string_view::filtered_string_view() noexcept
	: data_(nullptr)
	, size_(0)
	, pred_(default_predicate) {}
	// String constructor
	filtered_string_view::filtered_string_view(const std::string& s) noexcept
	: data_(s.data())
	, size_(s.size())
	, pred_(default_predicate) {}
	// String constructor with Predicate (lack some function should be fixed)
	filtered_string_view::filtered_string_view(const std::string& s, filter predicate) noexcept
	: data_(s.data())
	, size_(s.size())
	, pred_(std::move(predicate)) {}
	// Implicit Null-terminated String Constructor
	filtered_string_view::filtered_string_view(const char* s) noexcept
	: data_(s)
	, size_(std::strlen(s))
	, pred_(default_predicate) {}
	// Null-Terminated String with Predicate Constructor
	filtered_string_view::filtered_string_view(const char* s, filter predicate) noexcept
	: data_(s)
	, size_(std::strlen(s))
	, pred_(std::move(predicate)) {}
	// Copy constructor
	filtered_string_view::filtered_string_view(const filtered_string_view& other) noexcept = default;
	// Move constructor
	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: data_(std::exchange(other.data_, nullptr))
	, size_(std::exchange(other.size_, 0))
	, pred_(std::exchange(other.pred_, default_predicate)) {}
	// =
	filtered_string_view& filtered_string_view::operator=(const filtered_string_view& other) noexcept {
		// copy itself
		if (&other == this) {
			return *this;
		}
		this->data_ = other.data_;
		this->size_ = other.size_;
		this->pred_ = other.pred_;
		return *this;
	}
	filtered_string_view& filtered_string_view::operator=(filtered_string_view&& other) noexcept {
		this->data_ = std::exchange(other.data_, nullptr);
		this->size_ = std::exchange(other.size_, 0);
		this->pred_ = std::exchange(other.pred_, default_predicate);
		return *this;
	}
	// []
	const char& filtered_string_view::operator[](int n) const {
		int filtered_index = 0;
		for (size_t i = 0; i < size_; i++) {
			if (pred_(data_[i])) {
				if (filtered_index == n) {
					return data_[i];
				}
				filtered_index++;
			}
		}
		return data_[0];
	}

	// String type conversion
	filtered_string_view::operator std::string() const {
		std::string str = {};
		for (size_t i = 0; i < size_; i++) {
			if (pred_(data_[i])) {
				str.push_back(data_[i]);
			}
		}
		return str;
	}

	// get data or size function
	const char* filtered_string_view::data() const noexcept {
		return data_;
	}
	const filter& filtered_string_view::predicate() const noexcept {
		return pred_;
	}
	const char& filtered_string_view::at(int index) const {
		if (index < 0) {
			throw std::domain_error{"filtered_string_view::at(" + std::to_string(index) + "): invalid index"};
		}
		int n = 0;
		for (size_t i = 0; i < size_; i++) {
			if (pred_(data_[i])) {
				if (n == index) {
					return data_[i];
				}
				n++;
			}
		}
		throw std::domain_error{"filtered_string_view::at(" + std::to_string(index) + "): invalid index"};
	}
	std::size_t filtered_string_view::size() const {
		std::string str = {};
		for (size_t i = 0; i < size_; i++) {
			if (pred_(data_[i])) {
				str.push_back(data_[i]);
			}
		}
		return str.size();
	}
	// check empty
	bool filtered_string_view::empty() const noexcept {
		for (size_t i = 0; i < size_; i++) {
			if (pred_(data_[i])) {
				return false;
			}
		}
		return true;
	}

	// None member function
	filtered_string_view compose(const filtered_string_view& filtered_sv, const std::vector<filter>& filts) noexcept {
		filter new_pred = filtered_sv.predicate();
		for (const auto& fl : filts) {
			filter prev_pred = new_pred;
			new_pred = [prev_pred, fl](const char& c) { return prev_pred(c) && fl(c); };
		}
		return {filtered_sv.data(), new_pred};
	}
	std::vector<filtered_string_view> split(const filtered_string_view& fsv, const filtered_string_view& tok) {
		std::vector<filtered_string_view> result;
		auto start = fsv.begin();
		auto end = --fsv.end();
		auto tok_begin = tok.begin();
		auto tok_end = tok.end();
		auto it = fsv.begin();

		while (it != fsv.end()) {
			auto match = std::search(it, fsv.end(), tok_begin, tok_end);
			if (match == fsv.end()) {
				std::string fsv_str = {};
				for (auto i = start; i != fsv.end(); i++) {
					fsv_str += *i;
				}
				result.push_back({fsv_str, fsv.predicate()});
				if (match == end && !result.empty() && std::distance(tok_begin, tok_end) > 0) {
					result.push_back({"", fsv.predicate()});
				}
				break;
			}
			else if (match == end) {
				std::string fsv_str = {};
				for (auto i = start; i != end; i++) {
					fsv_str += *i;
				}
				result.push_back({fsv_str, fsv.predicate()});
				if (match == end && !result.empty() && std::distance(tok_begin, tok_end) > 0) {
					result.push_back({"", fsv.predicate()});
				}
				break;
			}
			std::string fsv_str2 = {};
			for (auto i = start; i != match; i++) {
				fsv_str2 += *i;
			}
			result.push_back({fsv_str2, fsv.predicate()});
			// update position
			it = match;
			start = match;
			std::advance(it, std::distance(tok_begin, tok_end));
			std::advance(start, std::distance(tok_begin, tok_end));
		}

		return result;
	}
	filtered_string_view substr(const filtered_string_view& fsv, int pos, int count) {
		int size = static_cast<int>(fsv.size());
		int rcount = (count <= 0 || count > size - pos) ? size - pos : count;
		if (pos < 0 || pos >= size) {
			return {"", fsv.predicate()};
		}

		auto start_it = fsv.begin();
		std::advance(start_it, pos);

		auto end_it = start_it;
		std::advance(end_it, rcount);

		return {fsv.data() + pos, [=](const char& c) { return &c >= &*start_it && &c < &*end_it && fsv.predicate()(c); }};
	}

	// None member operator
	// == != <==>
	bool operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) {
		std::string filtered_str1 = lhs.operator std::string();
		std::string filtered_str2 = rhs.operator std::string();
		return filtered_str1 == filtered_str2;
	}
	bool operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs) {
		return !(lhs == rhs);
	}
	std::strong_ordering operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) {
		std::string filtered_str1 = lhs.operator std::string();
		std::string filtered_str2 = rhs.operator std::string();
		return filtered_str1 <=> filtered_str2;
	}
	// <<
	std::ostream& operator<<(std::ostream& os, const filtered_string_view& filtered_sv) noexcept {
		os << static_cast<std::string>(filtered_sv);
		return os;
	}
} // namespace fsv
