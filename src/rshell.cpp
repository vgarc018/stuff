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
#include <boost/algorithm/string/trim_all.hpp>


using namespace std;
using namespace boost;

typedef tokenizer<char_separator<char> > mytok;
typedef mytok::iterator tok_it;

template <typename T>
void vec_print(vector<T> &s);
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
void removeWhite(vector<string> &v);
void piping(vector<string> &v, queue<string> &c);
int inRedirPiping(string s);


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
    removeWhite(v);
    //qprint(co);
    //vec_print(v);
    size_t input = line.find("<");
    size_t out = line.find(">");
    size_t p = line.find("|");
    size_t l = string::npos;
    if(p != l && line[p+1] != '|')
    {
      piping(v, co);
      continue;
    }
    else if(input != l)
    {
      inRedir(v);
      continue;
    }
    else if(out != l && (line[out+1] != '>'))
    {
      outRedir(v);
      continue;
    }
    else if(out != l)
    {
      outRedir2(v);
      continue;
    }
    else
    {
      int i = hand_connectors(v, co);
      if(i == 0)
        continue;
    }
  }
  return 0;
}

/*
 * Function to reomove the white space from every token
 * in the tokens vector so that when you put it into the 
 * function you wont get any error
 */
void removeWhite(vector<string> &v)
{
  for(size_t i = 0; i < v.size(); ++i)
    trim_all(v[i]);
  //vec_print(v);
}

/* Simple function to print the constents of a
 * vector type
 */
template <typename T>
void vec_print(vector<T> &s)
{
  for(size_t i = 0; i < s.size(); ++i)
    cout << "v[" << i << "] =  " << s[i] << endl;  
}

/*function that goes through a string and searched for any connectors
 */ 
void connectors(string s, queue<string> &c)
{
  for(size_t i = 0; i < s.size(); i++)
  {
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
      //cout << "in the queue" << endl;
      c.push("<");
    }
    else if(s[i] == '>' && s[i+1] != '>' && s[i-1] != '>')
    {
      c.push(">");
    }
    else if(s[i] == '>' && s[i+1] == '>')
    {
      c.push(">>");
    }
    else if(s[i] == '|' && s[i+1] != '|')
    {
      c.push("|");
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
  char_separator<char> connector(";||&&<>");
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
  //cout << file << endl;
  int infd = open(file.c_str(), O_RDONLY);
  if(infd == -1)
  { 
    perror("Error opening");
    return;
  }

   int savedIn = dup(STDIN_FILENO);
  if(savedIn == -1)
    perror("Error in dup");

  if(dup2(infd, STDIN_FILENO) == -1)
    perror("error in dup2");

  if(close(infd) == -1)
    perror("Error Closing fd");
  
  string command = v.at(0);

  execvp_connectors(command);
  if(dup2(savedIn, STDIN_FILENO) == -1)
    perror("Error in Dup2");

  if(close(savedIn) == -1)
    perror("Error in closing Fd");

}

void outRedir(vector<string> &v)
{
  string cmd = v.at(0);
  string file = v.at(1);

  int in = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if(in == -1)
  {  
    perror("Error in Opening");
    return;
  }
  
  int savedOut = dup(STDOUT_FILENO);
  if(savedOut == -1)
    perror("Error in dup");

  if(dup2(in, STDOUT_FILENO) == -1)
  { 
    perror("Error in Dup2");
    return;
  }

  if(close(in) == -1)
    perror("Closing fd");

  execvp_connectors(cmd);

  if(dup2(savedOut, STDOUT_FILENO) == -1)
    perror("Error in Dup2");

  if(close(savedOut) == -1)
    perror("Error Closing file");

}

void outRedir2(vector<string> &v)
{
  string cmd = v.at(0);
  string file = v.at(1);

  int in = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
  if(in == -1)
  {  
    perror("Error in Opening");
    return;
  }
  
  int savedOut = dup(STDOUT_FILENO);
  if(savedOut == -1)
    perror("Error in dup");

  if(dup2(in, STDOUT_FILENO) == -1)
  { 
    perror("Error in Dup2");
    return;
  }

  if(close(in) == -1)
    perror("Closing fd");

  execvp_connectors(cmd);

  if(dup2(savedOut, STDOUT_FILENO) == -1)
    perror("Error in Dup2");

  if(close(savedOut) == -1)
    perror("Error Closing file");

} 

int inRedirPiping(string s)
{
  int infd = open(s.c_str(), O_RDONLY);
  if(infd == -1)
  { 
    perror("Error opening");
    return -1;
  }
  return infd;
}

void piping(vector<string> &v, queue<string> &c)
{
  int savedIn = dup(STDIN_FILENO);
  if(savedIn == -1)
    perror("error in dup");
  int savedOut = dup(STDOUT_FILENO);
  if(savedOut == -1)
    perror("Error in dup");
  string top = c.front();
  int in;
  if(top == "<")
    in = inRedirPiping(v.at(1));
  else
    in = STDIN_FILENO;
  
  v.erase(v.begin()+1);
//  if()
  if(in == -1)
    return;
  
 // vec_print(v);
  
  int fd[2];
  size_t i;
  for(i = 0; i < v.size()-1; ++i)
  {
    cerr << "v[ " << i << " ]" << " = " << v[i] << endl;
    if(pipe(fd) == -1)
      perror("error in pipe");
    
    size_t pid = fork();
    
    
    if(pid == -1)
      perror("Error in fork");
    if(pid == 0)
    {
      if(in != STDIN_FILENO)
      {
      
        if(dup2(in, STDIN_FILENO) == -1)
        {
          perror("Error in Dup2 1");
          return;
        }
        if(close(in) == -1)
          perror("Error in Close");

      }
      if(dup2(fd[1], 1) == -1)
        perror("Error in dup2");

      if(close(fd[1]) == -1)
        perror("Error in Close");
       execvp_connectors(v.at(i));
    }
    else
    {
     if(close(fd[1]) == -1)
       perror("error in close");
     cerr  << "fd[0] ==  " <<  fd[0] << endl; 
     in = fd[0];
  cerr << " in before end loop" << in << endl;
    }
  }

  cerr << "in == " << in << endl;
  if(dup2(in, 0) == -1)
    perror("error in dup2 2");

  if(pipe(fd) == -1)
    perror("Error in pipe");

  size_t pid = fork();

  if(pid == -1)
    perror("Error in fork");

  if(pid == 0)
  {
    execvp_connectors(v.at(v.size() - 1));;
  }
  else
  {
    int x;
    if(wait(&x) == -1)
      perror("error in wait");

    if(dup2(savedOut, STDOUT_FILENO) == -1)
      perror("Error in Dup2");

    if(dup2(savedIn, STDIN_FILENO) == -1)
      perror("Error in Dup2");

    if(close(savedOut) == -1)
      perror("Error Closing file");

    if(close(savedIn) == -1)
      perror("Error Closing file");




  }

}
