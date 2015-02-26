SRCDIR=./src
BINDIR=./bin

all:
	@mkdir -p $(BINDIR)
	g++-4.9 --std=c++11 -Wall -Werror -ansi -pedantic $(SRCDIR)/rshell.cpp -o $(BINDIR)/rshell
	#g++ -Wall -Werror -ansi -pedantic $(SRCDIR)/ls.cpp -o $(BINDIR)/ls
	#g++ -Wall -Werror -ansi -pedantic $(SRCDIR)/cp.cpp -o $(BINDIR)/cp

clean:
	@rm -r $(BINDIR)
