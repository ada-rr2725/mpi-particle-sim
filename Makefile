CXX      = mpicxx
CXXFLAGS = -O2 -Wall
TARGET   = mpi-particle-sim
SRCS     = main.cpp particle.cpp domain.cpp
OBJS     = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) particles_p*.dat

.PHONY: clean
