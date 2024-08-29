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

int main(void) {
  DIR *dp = NULL;
  char buf[4096] = {""};
  char dir[4096] = {""};
  char *dirPtr = dir;
  struct stat st = {0};
  struct dirent *entry = NULL;     

  if (NULL == (dp = opendir("markdown"))){
    goto err;
  }
  snprintf(buf, sizeof(buf) - 1, "generated");
  if (stat(buf, &st) == -1) mkdir(buf, 0700);

  while ((entry = readdir(dp))) {
    if (*(entry->d_name) == '.') continue;

    dirPtr = dir;
    for (char *entryPtr = entry->d_name; *entryPtr; entryPtr++) {
      if (*entryPtr == '.' && *(entryPtr+1) == 'm') break;
      *dirPtr++ = *entryPtr;
    }
    *dirPtr = '\0';

    snprintf(buf, sizeof(buf) - 1, "generated/%s", dir);
    if (stat(buf, &st) == -1) mkdir(buf, 0700);

    snprintf(buf, sizeof(buf) - 1, "markdown/%s", entry->d_name);

    FILE *stream = fopen(buf, "rb");
    char *contents = "";
    long int fileSize = 0L;
    fseek(stream, 0L, SEEK_END);
    fileSize = ftell(stream);
    fseek(stream, 0L, SEEK_SET);
    contents = static_cast<char *>(malloc(5 * (fileSize + 1)));
    size_t size = fread(contents, 1, fileSize, stream);
    contents[size] = '\0'; 
    snprintf(buf, sizeof(buf) - 1, "generated/%s/index.html", dir);
    FILE *fp = fopen(buf, "w");
    if (!fp) {
	  fclose(stream);
	  free(contents);
	  goto err;
    }
    fprintf(fp, "%s", mdToHtml(static_cast<std::string>(contents)).c_str());
    fclose(fp);
    fclose(stream);
    free(contents);

    buf[0] = '\0';
    dir[0] = '\0';
  }
  if ((closedir(dp)) == -1) {
    goto err;
  }
  return EXIT_SUCCESS;
 
err:
  return EXIT_FAILURE;
}

std::string mdToHtml(const std::string &mdInput) {
    std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
    std::istringstream markdownStream(mdInput);
    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
    std::string htmlOutput = parser->Parse(markdownStream);
    return htmlOutput;
}

/*int main(void) {
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
      //puts(writeToIndex);
      outdata.open(writeToIndex);
      if (!outdata) { puts("nope."); break; }
      outdata << mdToHtml(strStream.str()) << std::endl;
      outdata.close();
      
    }
    openUpEntry.close();
    genEntryDir[0] = '\0';
    writeToIndex[0] = '\0';
  }

  std::cout << "Done\n" << std::flush;
  return EXIT_SUCCESS;
}*/
