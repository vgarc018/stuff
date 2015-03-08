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
#include <ostream>
#include <signal.h>

using namespace std;
using namespace boost;

typedef tokenizer<char_separator<char> > mytok;
typedef mytok::iterator tok_it;

template <typename T>
void vec_print(vector<T> &s);

void sig_handler(int signum);
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
int outRedirPiping(string s);
int outRedir2Piping(string s);
static inline std::string &rtrim(std::string &s);
static inline std::string &ltrim(std::string &s);
static inline std::string &trim(std::string &s);

int main()
{



  if( signal(SIGINT, SIG_IGN) == SIG_ERR )
      perror("Signal Error");

  string line;
  while(1)
  {
    cout << "\n";
    cout << flush;
    cout.flush();
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
    size_t semi = line.find(";");
    size_t orr = line.find("||");
    size_t andd = line.find("&&");
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
    else if(semi != l || orr != l || andd != l || !v.empty())
    {
      int i = hand_connectors(v, co);
      if(i == 0)
        continue;
    }
  }
  return 0;
}


void sig_handler(int signum)
{
    int pid = getpid();
    if(kill(pid, signum) == -1)
    {
        perror("Error in kill");
        exit(0);
    }
}
// trim from start
static inline std::string &ltrim(std::string &s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}
static inline std::string &rtrim(std::string &s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}
static inline std::string &trim(std::string &s)
{
  return ltrim(rtrim(s));
}


/*
 * Function to reomove the white space from every token
 * in the tokens vector so that when you put it into the
 * function you wont get any error
 */
void removeWhite(vector<string> &v)
{
  for(size_t i = 0; i < v.size(); ++i)
    trim(v[i]);
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

  char dir[1000];
  if(!getcwd(dir,1000)) perror("error in getwcd");

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
  if(cmds[0] == "cd")
  {

    //vec_print(cmds);
    if(cmds.size() == 1)
    {
      const char *home_name = "HOME";
      char *home = getenv(home_name);
      //cout << home << endl;
       int i = chdir(home);
      if(i == -1)
      {
        perror("Error in chdir");
        return -1;
      }
      return 0;
    }
    else
    {
    int i = chdir(cmds[1].c_str());
    if(i == -1)
    {
      perror("Error in chdir");
      return -1;
    }
    return 0;
    }
  }
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
      if(signal(SIGINT, sig_handler) == SIG_ERR)
          perror("Signal");
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
    if(signal(SIGINT, SIG_IGN) == SIG_ERR)
        perror("signal error");

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

int outRedirPiping(string s)
{
  int in = open(s.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if(in == -1)
  {
    perror("Error in Opening");
    return -1;
  }
  return in;
}

int outRedir2Piping(string s)
{
  int in = open(s.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
  if(in == -1)
  {
    perror("Error in Opening");
    return -1;
  }
  return in;
}

void piping(vector<string> &v, queue<string> &c)
{
  int savedIn = dup(0);
  if(savedIn == -1)
    perror("error in dup");
  int savedOut = dup(0);
  if(savedOut == -1)
    perror("Error in dup");
  string top = c.front();
  int in;
  if(top == "<")
  {
    in = inRedirPiping(v.at(1));
    v.erase(v.begin() +1);
  }
  else
    in = 0;

  if(in == -1)
    return;

  int output;
  if(c.back() == ">")
  {
    output = outRedirPiping(v[v.size()-1]);
    v.pop_back();
  }
  else if(c.back() == ">>")
  {
    output = outRedir2Piping(v[v.size()-1]);
    v.pop_back();
  }
  else
    output = 1;


  int fd[2];
  size_t i;
  for(i = 0; i < v.size()-1; ++i)
  {
    if(pipe(fd) == -1)
      perror("error in pipe");

    size_t pid = fork();
    size_t q = -1;

    if(pid == q)
      perror("Error in fork");
    if(pid == 0)
    {
      if(in != 0)
      {

        if(dup2(in, 0) == -1)
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
      exit(1);
    }
    else
    {
      if(close(fd[1]) == -1)
       perror("error in close");
      in = fd[0];
    }
  }

  if(dup2(in, 0) == -1)
    perror("error in dup2 2");

  size_t pid = fork();
  size_t q = -1;
  if(pid == q)
    perror("Error in fork");

  if(pid == 0)
  {
    if(output != STDOUT_FILENO)
    {
      if(dup2(output, 1) == -1)
         perror("Error in dup2 3");

      if(close(output) == -1)
        perror("Error in closing fd");
    }
    else
    {
      if(dup2(savedOut, STDOUT_FILENO) == -1)
        perror("Error in dup2");
    }
    execvp_connectors(v.at(v.size() - 1));
    //exit(0);
  }
  else
  {
    if(waitpid(pid, NULL, 0) == -1)
      perror("error in wait");
    exit(1);
  }

  if(dup2(savedOut, 1) == -1)
    perror("Error in Dup2");

  if(dup2(savedIn, 0) == -1)
    perror("Error in Dup2");

  if(close(savedOut) == -1)
    perror("Error Closing file");

  if(close(savedIn) == -1)
    perror("Error Closing file");

}
