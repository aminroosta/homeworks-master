#include<iostream>
#include<functional>
#include<fstream>
#include<set>
#include<map>

using namespace std;

/* T is the id and U is the price in this example*/
template<typename T, typename U>
struct orderbook {
	void insert(T t, U u) {
		/* keep the associated price for agiven id with log(n),
		   note: we need this when we want to delete the give id from our set */
		_map[t] = u;
		/* insert a pair<price,id> into the set with log(n) */
		_set.insert(UT(u, t));
	}
	void erase(T t) {
		UT ut = make_pair(_map[t], t); /* get the pair<price,id> */
		_map.erase(t);
		_set.erase(ut);
	}
	/* i am not using boost::optional here */
	bool try_get_highest_price(U& u) { /* returns true on success */
		if (_set.empty()) {
			u = U();
			return false;
		}
		u = _set.begin()->first;
		return true;
	}


private:
	/* set of pair<price,id> sorted by price */
	typedef pair<U, T> UT;
	set < UT, greater<UT> > _set;

	/* a map of ids to their prices */
	map<T, U> _map;
};


struct order {
	order() : timestamp(), operation(), id(), price() { /* set all values to zero */ }
	int timestamp;
	char operation;
	int id;
	double price;
};
istream& operator>> (istream& iss, order& order) {
	iss >> order.timestamp >> order.operation >> order.id;
	if (order.operation == 'I')
		iss >> order.price;
	else
		order.price = 0;
	return iss;
}

struct program {
	int total_duration; /* for calculating time weighted average price */
	double total_prices;
	int last_timestamp; /* we use -1 for to indicate an invalid value*/
	
	orderbook<int, double> orderbook;
	
	program() : total_duration(), total_prices(), last_timestamp(-1) { }

	void process(const order& order) {
		/* claculate the highest_price for the previous range before inserting or removing the new order*/
		double highest_price = 0;
		if (orderbook.try_get_highest_price(highest_price)) {
			int duration = order.timestamp - last_timestamp;
			total_duration += duration;
			total_prices += highest_price * duration;
		}

		if (order.operation == 'I') {
			orderbook.insert(order.id, order.price);
		}
		else {
			orderbook.erase(order.id);
		}

		/* update last timestamp */
		last_timestamp = order.timestamp;
	}

	double time_weighted_average() {
		if (total_duration == 0)
			return 0.0;
		return total_prices / total_duration;
	}
};

int main(int argc, char** argv) {
	ifstream iss(argv[1]);
	order order;
	program program;

	while (iss >> order) {
		program.process(order);
	}

	cout << "weighted average price = " << program.time_weighted_average() << endl;

	return 0;
};