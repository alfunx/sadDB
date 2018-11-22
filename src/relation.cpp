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

void Relation::join(const Relation& other)
{
	select(other.distinct_keys());

	for (auto it = records.begin(); it < records.end(); ++it)
	{
		for (const auto& r : other.records)
		{
			if (it->key != r.key)
				continue;

			for (const auto& a : r.payload)
				it->payload.push_back(a);
		}
	}
}

void Relation::union_all(const Relation& other)
{
	records.insert(records.end(),
			other.records.begin(),
			other.records.end());
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

Relation Relation::copy_select(int k) const
{
	Relation tmp;

	for (auto r : records)
	{
		if (r.key == k)
			tmp.records.push_back(r);
	}

	return tmp;
}

Relation Relation::copy_select(const std::set<int>& k) const
{
	Relation tmp;

	for (auto r : records)
	{
		if (k.find(r.key) == k.end())
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
		os << t << std::endl;

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
