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
  for(auto i = 0; i < s.size(); ++i)
    cout << "v[" << i << "] =  " << s[i] << endl;  
}

void connectors(string s, queue<string> &c)
{
  for(auto i = 0; i < s.size(); i++)
  {
    if(s[i] == '|' && s[i+1] == '|')
    {
      c.push("||");
    }
    else if(s[i] == ';')
    {
      c.push(";");
    }
    else if(s[i] == '&' && s[i+1] == '&')
    {
      c.push("&&");
    }
  }
}
template <typename T>
void qprint(queue<T> &c)
{
  while(!c.empty())
  {
    string s = c.front();
    cout << "c[  ]" << s << endl;
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
template <typename T>
void cat (vector<T> &s, string cmd)
{
  for(auto i = 0; i < s.size(); i++)
  {
    string temp = s[i] + "/" + cmd;
    s[i] = temp;
  }
}
void execvp_connectors(string s)
{
  const char *path_name = "PATH";
  char *path = getenv(path_name);
  if(path == NULL)
  {
    perror("error in getenv");
    exit(1);
  }
  string path_str(path);
  char_separator<char> colon(":");
  mytok tok (path_str, colon);
  vector<string> paths;
  for(auto i =tok.begin(); i != tok.end(); ++i)
  {
    paths.push_back(*i);
  }
  cat(paths, s);
  sort(paths.begin(), paths.end());
  print(paths);
}


int main()
{
  string line;
  vector<string> v;
  getline(cin, line);
  parsing(line, v);
  //print(v);
  queue<string> co;
  connectors(line, co);
  //qprint(co);
  execvp_connectors(v[0]);
  
  return 0;
}
