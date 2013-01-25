#include <iostream>
#include <fstream>
#include <cstdint>
#include <arpa/inet.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

class BinReader {
  ifstream &in;
public:
  BinReader(ifstream &is) : in(is) { }
  template <class T> BinReader& operator >> (T &x) {
    in.read(reinterpret_cast<char*>(&x), sizeof(T));
    return(*this);
  }
  BinReader& operator >> (string &x) {
    uint32_t n;
    (*this) >> n;
    char *buf = new char[n+1];
    in.read(buf, n);
    buf[n] = '\0';
    x = buf;
    delete buf;
    return(*this);
  }
};

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "USAGE: " << argv[0] << " prof.sav\n";
    return(1);
  }

  ifstream infile(argv[1], ios::binary);
  if (!infile.is_open()) {
    cerr << "Couldn't open file " << argv[0] << "\n";
    return(1);
  }

  BinReader b(infile);

  uint32_t version(0);
  b >> version;
  //cout << "Version: " << version << "\n";

  if (version != 4) {
    cerr << "Unsupported version: " << version << ". Need ver 4\n";
    return(1);
  }

  uint32_t n_ach(0);
  b >> n_ach;
  for(unsigned int i=0; i < n_ach; ++i) {
    string ach_name;
    uint32_t unk;
    b >> ach_name;
    b >> unk;
    //cout << ach_name << "\n";
  }
  
  uint32_t ship_unlocks[12];
  for(unsigned int i=0; i < 12; ++i)
    b >> ship_unlocks[i];
  
  unordered_map<string, int> won;
  unordered_map<string, string> type2name;
  vector<string> ship_types;

  for(int j=0; j < 2; ++j) {
    uint32_t n_hs;
    b >> n_hs;
    for(unsigned int i=0; i < n_hs; ++i) {
      string ship_name;
      string ship_type;
      b >> ship_name >> ship_type;
      uint32_t score; b >> score;
      uint32_t sector;  b >> sector;
      uint32_t victory; b >> victory;
      uint32_t easy; b >> easy;

      if (won.count(ship_type) == 0) {
	ship_types.push_back(ship_type);
	won[ship_type] = 0;
      }
      type2name[ship_type] = ship_name;

      
      if (victory) {
	if (easy) 
	  won[ship_type] |= 1;
	else
	  won[ship_type] |= 2;
      }
    }
  }
  infile.close();
  
  sort(ship_types.begin(), ship_types.end());

  cout << "Ship\tEasy\tNormal\n";
  for(unsigned int i=0; i < ship_types.size(); ++i) {
    cout << type2name[ship_types[i]] << "\t"
	 << (won[ship_types[i]] & 1) << "\t"
	 << ((won[ship_types[i]] & 2) >> 1) << "\n";
  }

}
