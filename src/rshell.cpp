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
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;
using namespace boost;

typedef tokenizer<char_separator<char> > mytok;
typedef mytok::iterator tok_it;

template <typename T>
void print(vector<T> &s);
void connectors(string s, queue<string> &c);
template <typename T>
void qprint(queue<T> c);
void parsing(string s, vector<string> &v);
template <typename T>
void cat (vector<T> &s, string cmd);
int execvp_connectors(string s);
int hand_connectors(vector<string> &s, queue<string> &c);
void inRedir(vector<string> &v);
void outRedir(vector<string> &v);
void outRedir2(vector<string> &v);

int main()
{
  string line;
  while(1)
  {
    char dir[1000];
    if(!getcwd(dir,1000)) perror("error in getwcd");
    char *login = getlogin();
    if(login == NULL) perror("Userlogin");
    char host[200];
    if(gethostname(host, 200) == -1) perror("hostname");

    cout << dir << endl;
    if(login) 
      cout << login << "@" << host << "$ ";
    else
      cout << "$ ";

    getline(cin, line);

    if(line[0] == '#') continue;
    size_t comment = line.find("#");
    if(comment != string::npos)
    {
      //cout << "in erase" << endl;
      line.erase(comment);
    }
    if(line == "exit")
    {
      cout << "Thanks for using Rshell" << endl;
      exit(0);
    }
    vector<string> v;
    parsing(line, v);
    queue<string> co;
    connectors(line, co);
    if(co.front() == "<" && co.size() == 1)
    {
      inRedir(v);
    }
    else
    {
      cerr << "in else" << endl;
      int i = hand_connectors(v, co);
      if(i == 0)
      continue;
    }
  }
  return 0;
}

template <typename T>
void print(vector<T> s)
{
  for(size_t i = 0; i < s.size(); ++i)
    cout << "v[" << i << "] =  " << s[i] << endl;  
}

void connectors(string s, queue<string> &c)
{
  for(size_t i = 0; i < s.size(); ++i)
  {
    cerr << "in for loop connectors" << endl;
    if(s[i] == '|' && s[i+1] == '|')
    {
      c.push("||");
    }
    else if(s[i] == '|' && s[i+1] != '|')
    {
      c.push("|");
    }
    else if(s[i] == '<')
    {
      c.push("<");
    }
    else if(s[i] == ';')
    {
      c.push(";");
    }
    else if(s[i] == '&' && s[i+1] == '&')
    {
      c.push("&&");
    }
    cerr << "at end" << endl;
  }
}
template <typename T>
void qprint(queue<T> c)
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
  char_separator<char> connector(";||&&<");
  mytok tok (s, connector);
  for(tok_it i = tok.begin(); i != tok.end(); ++i)
  {
    v.push_back(*i);
  }
}
template <typename T>
void cat (vector<T> &s, string cmd)
{
  for(size_t i = 0; i < s.size(); i++)
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
  for(tok_it i =tok.begin(); i != tok.end(); ++i)
  {
    paths.push_back(*i);
  }
  vector<string> cmds;
  char_separator<char> space (" ");
  mytok cmd_toks(s, space);
  for(tok_it i = cmd_toks.begin(); i != cmd_toks.end(); ++i)
  {
    cmds.push_back(*i);
  }
  cat(paths, cmds[0]);
  sort(paths.begin(), paths.end());
  //print(cmds);
  //cmds.erase(cmds.begin());
 // print(cmds);
 // print(paths);
  
  char **cm = (char**) malloc((cmds.size()+1) * sizeof(char*));
  if(cmds.size() != 0)
  {
    size_t k = 0;
    for(size_t i = 0; i < cmds.size(); ++i)
    {
      string temp = cmds[i];
      cm[k] = (char*) malloc((cmds[i].size()+1) * sizeof(char));
      strcpy(cm[k], temp.c_str());
      k++;
    }
    cm[k] = NULL;
  }
  size_t pid = fork();
  size_t err = -1;
  if(pid == err)
  {
    perror("fork Error");
  }
  if(pid == 0)
  {
      int exec;
      for(size_t i = 0; i < paths.size(); ++i)
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
    for(size_t i = 0; i < cmds.size()+1; ++i)
        free(cm[i]);
    free(cm);
    int x;
    if(wait(&x) == -1)
    {
      perror("Error in wait");
      exit(1);
    }
    return x;
  }
  return 6;
}

int hand_connectors(vector<string> &v, queue<string> &c)
{
  size_t diff = v.size() - 1;
  
  if(v[0] == "exit")
  {
    cout << "Thanks for using Rshell" << endl;
    exit(0);
  }

  if(v.size() == 1 && c.empty())
  {
    cerr << "in handler" << endl;
    int ret = execvp_connectors(v[0]);
    if(ret == 0)
      return 0;
    else
      return -1;
  }
  if(diff != c.size() && c.size() != v.size())
  {
    cout << "Please enter an acceptable command" << endl;
    return -1;
  }
  if(v.size()-1 == c.size())
  {
    c.push(";");
  }
  vector<string>::iterator it = v.begin();
  while(!c.empty())
  {
    //qprint(c);
    string conect = c.front();
    c.pop();
    if(*it == "exit" || *it == " exit")
    {
      cout << "Thanks for using Rshell" << endl;
      exit(0);
    }
    else if(conect == ";")
    {
      execvp_connectors(*it);
      if(c.empty()) return 0;
      ++it;
    }
    else if(conect == "&&")
    {
      int r = execvp_connectors(*it);
      if(r != 0) return -1;
      else
      {
        ++it;
      }
    }
    else if(conect == "||")
    {
      int r = execvp_connectors(*it);
      if(r == 0) 
      {
        if(c.front() == ";" || c.front() == "&&")
        {
          ++it;
          ++it;
          if(c.front() == "&&")
          {
            c.pop();
          }
        }
        else
        {
          return 0; 
        }
      }
      else
        ++it;
    }
  }
  
  return -1;
}





void inRedir(vector<string> &v)
{
  string cmd = v.at(0);
  string file = v.at(1);
  int infd = open(file.c_str(), O_RDONLY);
  if(infd == -1)
    perror("Error opening");

  int savedIn = dup(STDIN_FILENO);
  if(savedIn == -1)
    perror("Error in dup");

  if(dup2(infd, STDIN_FILENO) == -1)
    perror("error in dup2");

  if(close(infd) == -1)
    perror("Error Closing fd");
  
  string command = v.at(0) + " " + v.at(1);

  execvp_connectors(command);
  
  if(dup2(savedIn, STDIN_FILENO) == -1)
    perror("Error in Dup2");

  if(close(savedIn) == -1)
    perror("Error in closing Fd");

}
void outRedir(vector<string> &v)
{

}
void outRedir2(vector<string> &v)
{

}




