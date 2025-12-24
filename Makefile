CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread

# Консольные приложения
TASK1 = task1
TASK2 = task2
TASK3 = task3
TASK4 = task4
TASK5 = task5

# Qt приложения (требуют установленного Qt)
QT_CXXFLAGS = -fPIC $(shell pkg-config --cflags Qt5Widgets)
QT_LDFLAGS = $(shell pkg-config --libs Qt5Widgets)
MOC = moc

TASK6 = task6
TASK7 = task7

.PHONY: all clean console gui

all: console

console: $(TASK1) $(TASK2) $(TASK3) $(TASK4) $(TASK5)

gui: $(TASK6) $(TASK7)

# Консольные приложения
$(TASK1): task1.cpp
	$(CXX) $(CXXFLAGS) -o $(TASK1) task1.cpp

$(TASK2): task2.cpp
	$(CXX) $(CXXFLAGS) -o $(TASK2) task2.cpp

$(TASK3): task3.cpp
	$(CXX) $(CXXFLAGS) -o $(TASK3) task3.cpp

$(TASK4): task4.cpp
	$(CXX) $(CXXFLAGS) -o $(TASK4) task4.cpp

$(TASK5): task5.cpp
	$(CXX) $(CXXFLAGS) -o $(TASK5) task5.cpp

# Qt приложения
$(TASK6): task6.cpp
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -o $(TASK6) task6.cpp $(QT_LDFLAGS)

$(TASK7): task7.cpp
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -o $(TASK7) task7.cpp $(QT_LDFLAGS)

clean:
	rm -f $(TASK1) $(TASK2) $(TASK3) $(TASK4) $(TASK5) $(TASK6) $(TASK7) results.txt *.o

