#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
// #include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>

// #include "stlsl.h"

using namespace std;

void print(multimap<int, string> &m)
{
	multimap<int, string>::iterator it1 = m.begin();
	multimap<int, string>::iterator it2 = m.end();

	for(; it1!=it2; ++it1)
	{
		cout<<"key = "<<(it1->first)<<"\t\t";
		cout<<"value = "<<(it1->second)<<endl;
	}
	cout<<endl;
}

int main(int argc, char const *argv[]) {
  multimap<int, string> m1;
  m1.insert(pair<int, string>(1, "blob"));
  m1.insert(pair<int, string>(2, "blub"));
  m1.insert(pair<int, string>(3, "bleb"));
  m1.insert(pair<int, string>(1, "lsflkjslfjlks"));

  ofstream ofs("/tmp/serializer_poc.txt");
  boost::archive::text_oarchive serial1(ofs);
  serial1 << m1;
  ofs.close();

  ifstream ifs("/tmp/serializer_poc.txt");
  boost::archive::text_iarchive serial2(ifs);
  multimap<int, string> m2;
  serial2 >> m2;

  cout << "map1 = " << endl;
  print(m1);

  cout << endl;

  cout << "map2 = " << endl;
  print(m2);

  cout << "end" << endl;

  return 0;
}

/* Use: "g++ serialzer_poc.cpp -lboost_serialization" to compile!!!
   It is possible to write directly to the file you read from.
   See example: serialized_file_example.txt */
