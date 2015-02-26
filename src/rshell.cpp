#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <errno.h>
#include <stdio.h>
#include <boost/tokenizer.hpp>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <istream>
#include <queue>
#include <cstdio>

using namespace std;
using namespace boost;

typedef tokenizer<char_separator<char> > mytok;

template <typename T>
void print(vector<T> &s)
{
  for(auto i = 0; i <= s.size(); ++i)
  {
    cout << "v[" << i << "] =  " << s[i] << endl;  
  }
}

void connectors(string s, queue<string> &c)
{
  for(auto i = 0; i < s.size(); ++i)
  {
    if(s[i] == '|' && s[i+1] == '|')
    {
      c.push("||");
    }
    if(s[i] == ';')
    {
      c.push(";");
    }

    if(s[i] == '&' && s[i+1] == '&')
    {
      c.push("&&");
    }
  }
}
template <typename T>
void qprint(queue<T> &c)
{
  for(auto i = 0; i < c.size(); i++)
  {
    cout << "c[ " << i << "] = " << c.front() << endl;
    c.pop();
  }
}

void parsing(string s, vector<string> &v)
{
  char_separator<char> connector(";||&&");
  mytok tok (s, connector);
  for(auto i = tok.begin(); i != tok.end(); ++i)
  {
    v.push_back(*i);
  }
}


int main()
{
  string line;
  vector<string> v;
  getline(cin, line);
  parsing(line, v);
  print(v);
  queue<string> co;
  connectors(line, co);
  qprint(co);
  
  return 0;
}
