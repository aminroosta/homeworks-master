#include <vector>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

#define max_digits_allowed 1000

template <typename T> struct big_int; /* forward declaration */

template<typename T>
struct binary_wrapper { T const * value; };

template<typename T>
binary_wrapper<T> binary(T const & v) {
	binary_wrapper<T> bv;
	bv.value = &v;
	return bv;
}

/* print T in binary format */
template<typename T>
ostream& operator<<(ostream& os, binary_wrapper<T> bv) {
	vector<char> chars;
	int shift = sizeof(T) * 8 / 2;
	T value = *(bv.value);
	for (int i = 0; i < shift; ++i) {
		chars.push_back((value % 2) + '0');
		value /= 2;
	}
	for (int i = chars.size() - 1; i >= 0; --i)
		os << chars[i];
	return os;
}

template<typename T>
ostream& operator<<(ostream& os, binary_wrapper<big_int<T> > bi_warpper) {
	const big_int<T>* bi = bi_warpper.value;
	for (int i = bi->arr.size() - 1; i >= 0; --i)
		os << binary(bi->arr[i]);
	return os;
}

template <typename T>
struct big_int {
	vector<T> arr;
	static const int shift = sizeof(T)*8/2;
	big_int(T initial = 0) {
		arr.push_back(0);
		*this += initial;
	}
	big_int(const big_int<T>& initial) {
		arr = initial.arr;
	}

	static T upper(T v) {
		return v >> shift;
	}
	static T& clean_upper(T& v) {
		return v &= (1 << shift) - 1;
	}

	/* add a number to big_int */
	big_int<T>& operator +=(T rhs) {
		T carry = rhs;
		int i;
		for (i = 0; i < arr.size() && carry > 0; ++i) {
			arr[i] += carry;
			carry = upper(arr[i]);
			clean_upper(arr[i]);
		}
		if (i == arr.size() && carry > 0)
			arr.push_back(carry);
		return *this;
	}

	big_int<T>& operator=(const big_int<T>& rhs) {
		if (this == &rhs) return *this;
		arr = rhs.arr; // copy the other array
		return *this;
	}
	big_int<T>& operator+=(const big_int<T>& rhs) {
		T carry = 0;
		int till = arr.size() > rhs.arr.size() ? arr.size() : rhs.arr.size();
		for (int i = 0; i < till; i++) {
			T elem = carry + (i < arr.size() ? arr[i] : 0) + (i < rhs.arr.size() ? rhs.arr[i] : 0);
			carry = upper(elem);
			clean_upper(elem);
			if ( i < arr.size()) arr[i] = elem;
			else arr.push_back(elem);
		}
		if (carry > 0) arr.push_back(carry);
		return *this;
	}
	static big_int<T> from_power_two(int pw) {
		big_int<T> ret; ret.arr.pop_back();
		for (int i = 0; i < pw / shift; ++i)
			ret.arr.push_back(0);
		ret.arr.push_back(1ULL << (pw%shift));
		return ret;
	}
	big_int<T>& operator>>=(int degs) {
		T carry = 0;
		for (int i = arr.size() - 1; i >= 0; --i) {
			T elem = arr[i];
			T extra = elem & ((1ULL << degs) - 1);
			elem >>= degs;
			elem |= carry;
			arr[i] = elem;
			carry = extra << (shift - degs);
		}
		while (!arr.back()) arr.pop_back();
		return *this;
	}

	bool is_multiplicant_of(const big_int<T>& A) const {
		big_int<T> low = 1, high = from_power_two(arr.size() * shift);
		big_int<T> cur = 0;
		while (true) {
			cur = (low + high);
			cur >>= 1; /* divide by two */
			if (cur == low) break;
			//big_int<T> res = cur*A;
			//cout << low << ' ' << high << " cur:" << cur << " res:" << res << endl;
			//cout << res << " <= " << *this << " => " << (res <= *this) << endl;

			if (cur*A <= *this)
				low = cur;
			else
				high = cur;
		}
		return cur*A == *this;
	}

	big_int sqrt() const {
		big_int<T> low = 1, high = from_power_two(arr.size() * shift);
		big_int<T> cur = 0;
		while (true) {
			cur = (low + high);
			cur >>= 1; /* divide by two */
			if (cur == low)
				return cur;

			if (cur*cur <= *this)
				low = cur;
			else
				high = cur;
		}
		return cur;
	}
};

template<typename T>
big_int<T> operator+(const big_int<T>& A, const big_int<T>& B) {
	big_int<T> ret = A;
	ret += B;
	return ret;
}

template<typename T>
big_int<T> multiply_helper(const big_int<T>& A, T value, int position) {
	big_int<T> ret = 0; ret.arr.pop_back();
	for (int i = 0; i < position; ++i) ret.arr.push_back(0);
	T carry = 0;
	for (int i = 0, till = A.arr.size(); i < till; ++i) {
		T elem = value * A.arr[i] + carry;
		carry = big_int<T>::upper(elem);
		big_int<T>::clean_upper(elem);
		ret.arr.push_back(elem);
	}
	if (carry > 0) ret.arr.push_back(carry);
	return ret;
}

template<typename T>
big_int<T> operator*(const big_int<T>& A, const big_int<T>& B) {
	big_int<T> ret = 0;
	//for (int i = 0, till = B.arr.size(); i < till; ++i)
		//ret += multiply_helper(A, B.arr[i], i);
	#pragma region
	/* when i wrote this i and god knew what i was writing, NOW ONLY GOD KNOWS */
	for (int i = 0, till = B.arr.size(); i < till; ++i) {
		T value = B.arr[i];
		if (!value) continue;
		T carry = 0;
		for (int j = 0, tillj = A.arr.size(); j < tillj; ++j) {
			T elem = value * A.arr[j] + carry;
			carry = big_int<T>::upper(elem);
			big_int<T>::clean_upper(elem);

			T carry_ret = elem;
			T position = i + j;
			while (carry_ret) {
				while (ret.arr.size() <= position) ret.arr.push_back(0);
				T elem = ret.arr[position] + carry_ret;
				carry_ret = big_int<T>::upper(elem);
				big_int<T>::clean_upper(elem);
				ret.arr[position] = elem;
				++position;
			}
		}
		if (carry > 0) {
			int position = i + A.arr.size();
			T carry_ret = carry;
			while (carry_ret) {
				while (ret.arr.size() <= position) ret.arr.push_back(0);
				T elem = ret.arr[position] + carry_ret;
				carry_ret = big_int<T>::upper(elem);
				big_int<T>::clean_upper(elem);
				ret.arr[position] = elem;
				++position;
			}
		}
	}
	#pragma endregion
	return ret;
}

template<typename T>
bool operator<=(const big_int<T>& A, const big_int<T>& B) {
	if (A.arr.size() != B.arr.size())
		return A.arr.size() < B.arr.size();
	for (int i = A.arr.size() - 1; i >= 0; --i)
		if (A.arr[i] != B.arr[i])
			return A.arr[i] < B.arr[i];
	/* they are equal */
	return true;
}
template<typename T>
bool operator==(const big_int<T>& A, const big_int<T>& B) {
	if (A.arr.size() != B.arr.size())
		return false;
	for (int i = A.arr.size() - 1; i >= 0; --i)
		if (A.arr[i] != B.arr[i])
			return false;
	/* they are equal */
	return true;
}
template<typename T>
bool operator!=(const big_int<T>& A, const big_int<T>& B) {
	return !(A == B);
}

/* adds a string of numbers to another string of numbers*/
string add(const string& A, const string& B) {
	vector<char> digits;
	int Asize = A.size(), Bsize = B.size();
	int i = 0, till = Asize > Bsize ? Asize : Bsize;
	int carry = 0;
	for (i = 1; i <= till; ++i) {
		carry += (i <= Asize ? (A[Asize - i] - '0') : 0) + (i <= Bsize ? (B[Bsize - i] - '0') : 0);
		digits.push_back(carry%10 + '0');
		carry /= 10;
	}
	if (carry) digits.push_back(carry + '0');
	return string(digits.rbegin(), digits.rend());
}

/* returns the string representation of 2^pw */
const string& power_two(int pw) {
	static vector<string> powers; // = {"1", "2", "4", "8", "16", "32", "64", "128", "256", ... };
	static bool initialized = false;
	if (!initialized) {
		powers.push_back("1");
		for (int i = 1; i < max_digits_allowed; ++i)
			powers.push_back(add(powers.back(), powers.back()));
		initialized = true;
	}
	return powers[pw];
}

template<typename T>
ostream& operator <<(ostream& os, const big_int<T> & bi) {
	string res = "0";
	int digit_pos = 0, till = bi.arr.size() * bi.shift;
	for (digit_pos = 0; digit_pos < till; ++digit_pos) {
		if (bi.arr[digit_pos / bi.shift] & (1ULL << (digit_pos%bi.shift))) {
			res = add(res, power_two(digit_pos));
			//cout << power_two[digit_pos] << ',';
		}
	}
	os << res;
	return os;
}

void test_one() {
	big_int<uint8> bi = 0;
	uint32 sum = 0;
	for (int i = 1; i <= 10; ++i) {
		bi += (uint8)i;
		sum += i;
		cout << i << " => sum=" << sum << "   big_int=" << binary(bi) << "   sum=" << binary(sum) << endl;
		cout << bi << endl;

	}
}

void test_two() {
	big_int<uint8> bi = 150;
	for (int i = 0; i < 250; ++i)
		cout << (bi + big_int<uint8>(i)) << endl;
}

void test_three() {
	big_int<uint8> bi = 180;
	while (bi <= big_int<uint8>(200)) {
		bi += 1;
		cout << bi << endl;
	}
}

void test_four() {
	for (big_int<uint8> bi = 1, till = big_int<uint8>(20); bi <= till; bi += 1)
		for (big_int<uint8> bj = 1, tillj = big_int<uint8>(17); bj <= tillj; bj += 1)
			cout << bi << '*' << bj << '=' << bi*bj << endl;
}

void test_five() {
	for (int i = 0; i < 20; ++i) {
		big_int<uint8> bi = big_int<uint8>::from_power_two(i);
		cout << bi << ' ';
		cout << (bi >>= 1) << endl;
	}
}

void test_six() {
	big_int<uint8> bi = (210) /* 210 = 2*3*5*7 */;
	cout << "multiplicants of " << bi << " are : " << endl;
	for (big_int<uint8> bj = 1, till = 35; bj <= till; bj += 1) {
		if (bi.is_multiplicant_of(bj))
			cout << bj << ' ';
	}
	cout << endl;
}

template<typename T>
bool is_prime(const big_int<T>& A) {
	T to_add = 4;
	big_int<T> sq = A.sqrt();
	for(big_int<T> bi = 5; bi <= sq; bi += to_add) {
		if (A.is_multiplicant_of(bi))
			return false;
		to_add = (to_add == 2) ? 4 : 2;
	}
	return true;
}

void test_seven() {
	big_int<uint8> till = 100;
	//till = till * till; // 10,000

	uint8 to_add = 4;
	for (big_int<uint8> bi = 5; bi <= till; bi += to_add) {
		if (is_prime(bi))
			cout << bi << endl;
		to_add = (to_add == 2) ? 4 : 2;
	}
}

