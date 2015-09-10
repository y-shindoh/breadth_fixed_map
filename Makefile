# -*- coding: utf-8; tab-width: 4 -*-
# Author: Yasutaka SHINDOH / 新堂 安孝

SOURCE	:= main.cpp
HEADER	:= breadth_fixed_map.hpp
EXECUTE	:= breadth_fixed_map.exe

CXX			:= clang++
CXXFLAGS	:= -Wall -Weffc++ -O2 -std=c++11

check: $(EXECUTE)
	# check: $(<F)
	./$(EXECUTE) $(NUMBER)

all: $(EXECUTE)

$(EXECUTE): $(SOURCE) $(HEADER)
	# create: $(@F)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $@

clean:
	rm -f $(EXECUTE)
	find . -name '*~' -print0 | xargs -0 rm -f

.PHONY: all check clean
