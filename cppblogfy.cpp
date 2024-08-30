#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <regex>
#include <unistd.h>
#include <dirent.h>
#include <cctype>
#include <fcntl.h>
#include <sys/stat.h>

#include "maddy/parser.h"

std::string mdToHtml(const std::string &mdInput);

std::string mdToHtml(const std::string &mdInput) {
    std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
    std::istringstream markdownStream(mdInput);
    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
    std::string htmlOutput = parser->Parse(markdownStream);
    return htmlOutput;
}

int main(void) {
  const std::filesystem::path generated{"generated"};
  const std::filesystem::path mdSrc{"markdown"};
  std::filesystem::create_directories(generated);
 
  for (auto const &dir_entry : std::filesystem::recursive_directory_iterator{mdSrc}) {
    if (dir_entry.path().filename().string() == "." || dir_entry.path().filename().string() == "..") continue;

    char genEntryDir[4096] = {'\0'};
    char writeToIndex[4096] = {'\0'};
    snprintf(genEntryDir, sizeof(genEntryDir) - 1, "generated/%s", dir_entry.path().filename().string().c_str());
    snprintf(writeToIndex, sizeof(writeToIndex) - 1, "%s/index.html", genEntryDir);
    std::string mdEntry = std::regex_replace(genEntryDir, std::regex(".md"), "");
    std::filesystem::create_directories(mdEntry);

    std::string openMd = "markdown/" + dir_entry.path().filename().string(); 
    std::ifstream openUpEntry(openMd);
    std::stringstream strStream;
    std::ofstream outdata;
    if(openUpEntry.is_open()) {
      strStream << openUpEntry.rdbuf();
      char *ptr = writeToIndex;
      char buf2[4096] = {'\0'};
      char *bufPtr = buf2;
      for (; *ptr; ptr++) {
        if (*ptr == '.' && *(ptr + 1) == 'm') { ptr++; ptr++; continue; }
        *bufPtr++ = *ptr;
      }
      *bufPtr = '\0';
      puts(buf2);
      outdata.open(buf2);
      if (!outdata) { puts("Could not open file for writing."); break; }
      outdata << mdToHtml(strStream.str()) << std::endl;
      outdata.close();
      
    }
    openUpEntry.close();
    genEntryDir[0] = '\0';
    writeToIndex[0] = '\0';
  }

  puts("Done");
  return EXIT_SUCCESS;
}