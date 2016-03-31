TESTS= \
  testConnectionLifeTime \
  testNotifyPerformance \
  testQtNotifyPerformance

CFLAGS=-O2 -std=c++11 -I. -Wall -pthread -pipe
QTCORE_CFLAGS=$(CFLAGS) `pkg-config --cflags Qt5Core`
QTCORE_LFLAGS=-fPIC `pkg-config --libs Qt5Core`

all: $(TESTS)

testConnectionLifeTime: testConnectionLifeTime.cc cc/*.hh Makefile
	$(CXX) $(CFLAGS) $< -o $@

testNotifyPerformance: testNotifyPerformance.cc cc/*.hh Makefile
	$(CXX) $(CFLAGS) $< -o $@

moc_%.cc: %.cc Makefile
	moc $< > $@

testQtNotifyPerformance: testQtNotifyPerformance.cc moc_testQtNotifyPerformance.cc cc/*.hh Makefile
	$(CXX) $(QTCORE_CFLAGS) $< -o $@ $(QTCORE_LFLAGS)

clean:
	rm -rf moc_* $(TESTS)
