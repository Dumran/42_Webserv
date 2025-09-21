NAME = webserv

CXX         := g++
CXXFLAGS    := -std=c++98 -Wall -Wextra -Werror

RM = rm -rf

INCLUDE_DIR := include
CPPFLAGS    := -I$(INCLUDE_DIR)

SRCS        := $(shell find src -type f -name '*.cpp')
OBJS        := $(SRCS:src/%.cpp=build/%.o)
DEPS        := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking $(NAME)"
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	@echo "Cleaning object files"
	@rm -rf build

fclean: clean
	@echo "Removing binary $(NAME)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)
