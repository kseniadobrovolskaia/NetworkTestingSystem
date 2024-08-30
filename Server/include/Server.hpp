#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <sstream>

#include "TestSystem.h"

void failWithError(const char *Message);

int calculateMark(std::vector<unsigned> final_results, std::vector<unsigned> right_answers);

void createAction();

void saveAction();

void runFromFileAction();

void chooseAction();

