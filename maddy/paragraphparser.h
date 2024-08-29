/*
 * This project is licensed under the MIT license. For more information see the
 * LICENSE file.
 */
#pragma once

// -----------------------------------------------------------------------------

#include <functional>
#include <string>

#include "blockparser.h"

// -----------------------------------------------------------------------------

namespace maddy {

// -----------------------------------------------------------------------------

/**
 * ParagraphParser
 *
 * @class
 */
class ParagraphParser : public BlockParser
{
public:
  /**
   * ctor
   *
   * @method
   * @param {std::function<void(std::string&)>} parseLineCallback
   * @param {std::function<std::shared_ptr<BlockParser>(const std::string& line)>} getBlockParserForLineCallback
   */
   ParagraphParser(
    std::function<void(std::string&)> parseLineCallback,
    std::function<std::shared_ptr<BlockParser>(const std::string& line)> getBlockParserForLineCallback,
    bool isEnabled
  )
    : BlockParser(parseLineCallback, getBlockParserForLineCallback)
    , isStarted(false)
    , isFinished(false)
    , isEnabled(isEnabled)
  {}

  /**
   * IsStartingLine
   *
   * If the line is not empty, it will be a paragraph.
   *
   * This block parser has to always run as the last one!
   *
   * @method
   * @param {const std::string&} line
   * @return {bool}
   */
  static bool
  IsStartingLine(const std::string& line)
  {
    return !line.empty();
  }

  /**
   * IsFinished
   *
   * An empty line will end the paragraph.
   *
   * @method
   * @return {bool}
   */
  bool
  IsFinished() const override
  {
    return this->isFinished;
  }

protected:
  bool
  isInlineBlockAllowed() const override
  {
    return false;
  }

  bool
  isLineParserAllowed() const override
  {
    return true;
  }

  void
  parseBlock(std::string& line) override
  {
    if (this->isEnabled && !this->isStarted)
    {
      line = "<p>" + line + " ";
      this->isStarted = true;
      return;
    }
    else if (!this->isEnabled && !this->isStarted)
    {
      line += " ";
      this->isStarted = true;
      return;
    }

    if (this->isEnabled && line.empty())
    {
      line += "</p>";
      this->isFinished = true;
      return;
    }
    else if (!this->isEnabled && line.empty())
    {
      line += "<br/>";
      this->isFinished = true;
      return;
    }

    line += " ";
  }

private:
  bool isStarted;
  bool isFinished;
  bool isEnabled;
}; // class ParagraphParser

// -----------------------------------------------------------------------------

} // namespace maddy
