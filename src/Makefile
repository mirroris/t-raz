
# sources
SRCS:= engine.cpp

# compiler
CC:= g++ -g

# option
CFLAG = -Wall -m64 -std=c++17 

MOPT = MMD MP

# include PATH
INC = -Ic:/VulkanSDK/1.3.250.0/Include -Ic:/Libraries/glfw-3.3.8.bin.WIN64/include -Ic:/Libraries/glm
	
# Library
LIB = -Lc:/VulkanSDK/1.3.250.0/Lib -Lc:/Libraries/glfw-3.3.8.bin.WIN64/lib-mingw-w64 -Lc:/Libraries/glm
	

# File 

# object file
ALL_OBJ = $(patsubst %, %.o, $(ALL))
APP_OBJ = $(patsubst %, %.o, $(APP))
COMPONENT_OBJ = component/$(patsubst %, %.o, $(COMPONENT))

# source file
ALL_CPP = $(ALL:% = %.o)
APP_CPP = $(APP:% = %.o)
COMPONENT_CPP = component$(COMPONENT:% = %.o)


.PHONY: build

build: main.o
	$(CC) $(CFLAG) $(LIB) main.o -o main.exe -lvulkan-1 -lglfw3 -lgdi32

%.o: %.cpp
	$(CC) $(CFLAG) $(INC) -o $@ -c $< 
clean: 
	rm -rf $(ALL_OBJ)
