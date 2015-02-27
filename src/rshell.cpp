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

int execvp_connectors(string s)
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
  vector<string> cmds;
  char_separator<char> space (" ");
  mytok cmd_toks(s, space);
  for(auto i = cmd_toks.begin(); i != cmd_toks.end(); ++i)
  {
   cmds.push_back(*i);
  }
  cat(paths, cmds[0]);
  sort(paths.begin(), paths.end());
  //print(cmds);
  cmds.erase(cmds.begin());
 // print(cmds);
  print(paths);
  
  char** cm = static_cast<(char*)>[cmds.size()+1];
  size_t k = 0;
  for(auto i = 0; i < cmds.size(); ++i)
  {
    string temp = cmds[i];
    cm[k] = new char[temp.size()];
    strcpy(cm[k], temp.c_str());
    k++;
  }
  cm[k] = NULL;

  size_t pid = fork();
  size_t err = -1;
  if(pid == err)
  {
    perror("fork Error");
  }
  if(pid == 0)
  {
      int exec;
      for(auto i = 0; i < paths.size(); ++i)
      {
        exec = execv(paths[i].c_str(), cm);
      }
      if(exec == -1)
      {
        perror("error in Execv");
        exit(1);
      }
  }
  else
  {
    int x;
    if(wait(&x) == -1)
    {
      perror("Error in wait");
      exit(1);
    }
    return x;
  }
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
