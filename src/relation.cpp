#include "relation.h"

std::vector<Record> records;

Relation::Relation()
{
	/* void */
}

Relation::Relation(std::vector<Record> r) :
	records(r)
{
	/* void */
}

Relation Relation::copy() const
{
	Relation tmp;

	for (auto r : records)
		tmp.records.push_back(r);

	return tmp;
}

void Relation::insert(const Record& r)
{
	records.push_back(r);
}

void Relation::join(const Relation& a)
{
	auto keys = distinct_keys();
	auto keys_2 = a.distinct_keys();
	keys.insert(keys_2.begin(), keys_2.end());

	Relation tmp = Relation::select(a, keys);
	select(keys);

	for (auto it = records.begin(); it < records.end(); ++it)
	{
		for (const auto& r : tmp.records)
		{
			if (it->key != r.key)
				continue;

			for (const auto& p : r.payload)
				it->payload.push_back(p);
		}
	}
}

void Relation::union_all(const Relation& a)
{
	records.insert(records.end(),
			a.records.begin(),
			a.records.end());
}

void Relation::select(int k)
{
	for (auto it = records.begin(); it < records.end(); ++it)
	{
		if (it->key != k)
			records.erase(it);
	}
}

void Relation::select(const std::set<int>& k)
{
	for (auto it = records.begin(); it < records.end(); ++it)
	{
		if (k.find(it->key) == k.end())
			records.erase(it);
	}
}

Relation Relation::join(const Relation& a, const Relation& b)
{
	auto keys = a.distinct_keys();
	auto keys_2 = a.distinct_keys();
	keys.insert(keys_2.begin(), keys_2.end());

	Relation a1 = Relation::select(a, keys);
	Relation b1 = Relation::select(b, keys);
	Relation tmp;

	for (const auto& r1 : a1.records)
	{
		for (const auto& r2 : b1.records)
		{
			if (r1.key != r2.key)
				continue;

			Record r;
			r.key = r1.key;

			for (const auto& p : r1.payload)
				r.payload.push_back(p);

			for (const auto& p : r2.payload)
				r.payload.push_back(p);

			tmp.insert(r);
		}
	}

	return tmp;
}

Relation Relation::union_all(const Relation& a, const Relation& b)
{
	Relation tmp = a.copy();
	tmp.union_all(b);
	return tmp;
}

Relation Relation::select(const Relation& a, int k)
{
	Relation tmp;

	for (auto r : a.records)
	{
		if (r.key == k)
			tmp.records.push_back(r);
	}

	return tmp;
}

Relation Relation::select(const Relation& a, const std::set<int>& k)
{
	Relation tmp;

	for (auto r : a.records)
	{
		if (k.find(r.key) != k.end())
			tmp.records.push_back(r);
	}

	return tmp;
}

unsigned int Relation::count_keys(int k) const
{
	unsigned int count = 0;
	for (const auto& r : records)
		if (k == r.key)
			++count;
	return count;
}

unsigned int Relation::count() const
{
	return records.size();
}

std::set<int> Relation::distinct_keys() const
{
	std::set<int> keys;
	for (const auto& r : records)
		keys.insert(r.key);
	return keys;
}

std::ostream& operator <<(std::ostream& os, Relation r)
{
	for (const auto& t : r.records)
		os << t << '\n';

	return os;
}

std::istream& operator >>(std::istream& is, Relation& r)
{
	while (is)
	{
		Record record;
		is >> record;

		if (!is.bad())
			r.records.push_back(record);
	}

	return is;
}

Relation::Type Relation::other(Type type)
{
	return (Type)((type + 1) % 2);
}
