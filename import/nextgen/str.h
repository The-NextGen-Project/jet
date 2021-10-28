# ifndef NEXTGEN_STR_H
# define NEXTGEN_STR_H

# include "core-util.h"
# include "allocator.h"

namespace nextgen {
	using namespace core; using namespace nextgen::mem;

	template<typename T>
	struct Range {

		T begin;
		T end;

		Range(T begin, T end) : begin(begin), end(end) {}

		Range() = default;

		[[nodiscard]]
		NG_INLINE size_t range() const {
			return (size_t) (end - begin);
		}
	};


	class str {
	public:
		str() = default;

		struct intern_hash { // std::unordered_map<str, str::intern_hash>
			size_t operator()(str const &s) const {
				return s.getHashCache();
			}
		};

		struct intern_eq {
			bool operator()(str const &LHS, str const &RHS) const {
				return strncmp(LHS.pointer, RHS.pointer, LHS.len) == 0;
			}
		};


		template<unsigned long N>
		/*implicit*/
		constexpr str(const char(&data)[N]) : len(N), pointer(data) {}

		/*implicit*/ str(const char *data) : len(strlen(data)), pointer(data) {}

		/*implicit*/ str(char c) : len(1) { pointer = &c; }

		explicit str(const char *data, size_t len) : len(len), pointer(data) {}

		/*implicit*/ str(Range<const char *> range)
				: len(range.range()), pointer(range.begin) {}

		/*implicit*/ str(std::string &data) { // NOTE: Do not use init-list for this
			len = data.length();
			pointer = data.c_str();
		}

		[[nodiscard]]
		size_t size() const { return len; }

		// String FNV-1a Hashing Algorithm (Internal)
		[[nodiscard]]
		size_t hash() const {
			auto val = FNV_OFF;
			for (auto i = 0; i < len; ++i) {
				val ^= (pointer[i]);
				val *= FNV_PRIME;
			}
			return val;
		}

		bool operator==(str const &other) const {
			return pointer == other.pointer;
		}

		char operator[](size_t index) const {
			return (static_cast<const char *>(pointer))[index];
		}

		/*implicit*/ operator std::string() const {
			return std::string((const char *) *this);
		}

		/*implicit*/ operator Range<const char *>() const {
			return Range<const char *>(pointer, pointer + len);
		}

		explicit operator const char *() const {
			return static_cast<const char *>(pointer);
		}


		str operator-(const size_t size) const {
			return str{
					pointer - size,
					len - size
			};
		}

		str operator+(int offset) const {
			return str{
					pointer + offset,
					len + offset
			};
		}

		char operator*() const {
			return *pointer;
		}

		const char *begin() const {
			return pointer;
		}

		const char *end() const {
			return pointer + len;
		}

		// You MUST set the hash value before interning a string,
		// so it is not recomputed in str::intern_hash.
		NG_AINLINE size_t getHashCache() const {
			return hash_cache;
		}

		// Mutation.

		NG_INLINE void setHash() {
			hash_cache = hash();
		}

		NG_INLINE void setHash(size_t hash) {
			hash_cache = hash;
		}

		str operator++() {
			pointer++;
			return *this;
		}

		str operator--() {
			pointer--;
			return *this;
		}

		str operator+=(int offset) {
			pointer += (size_t) (offset);
			return *this;
		}

		str operator-=(int offset) {
			pointer -= (size_t) (offset);
			return *this;
		}

		const char *pointer{};     // char* data
		//bool is_heap_allocated = false;
	private:
		size_t len{}; // String length
		size_t hash_cache = 0;
	};


	// Very thin wrapper around std::unordered_set for String interning.
	// Mostly O(1) Retrieval (FNV-1a is fast and good String hash).
	// Designed around the symbols retrieved for the token and not the token
	// values.
	class StringCache {
	public:
		using StringSet = std::unordered_set<str, str::intern_hash, str::intern_eq>;

		static StringSet &Strings() {
			static StringSet Strings;
			return Strings;
		}

		static str &Get(str &s) {
			return const_cast<str &>(*(Strings().find(s)));
		}

		static NG_INLINE str InsertOrGet(str &s) {
			auto find = Strings().find(s);
			if (find != Strings().end()) {
				return *find;
			} else {
				auto new_string = (str) s;
				new_string.setHash(s.getHashCache());
				return (*Strings().insert(new_string).first);
			}
		}
	};

	str operator ""_intern(const char *s, size_t len);

	std::ostream &operator<<(std::ostream &s, nextgen::str &str);
}

#endif //NEXTGEN_STR_H
