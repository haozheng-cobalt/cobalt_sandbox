/*
 * Copyright 2014 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cobalt/css_parser/parser.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>

#include "base/bind.h"
#include "base/string_piece.h"
#include "base/string_util.h"
#include "base/time.h"
#include "cobalt/css_parser/grammar.h"
#include "cobalt/css_parser/margin_or_padding_shorthand.h"
#include "cobalt/css_parser/property_declaration.h"
#include "cobalt/css_parser/ref_counted_util.h"
#include "cobalt/css_parser/scanner.h"
#include "cobalt/css_parser/string_pool.h"
#include "cobalt/css_parser/trivial_string_piece.h"
#include "cobalt/css_parser/trivial_type_pairs.h"
#include "cobalt/cssom/after_pseudo_element.h"
#include "cobalt/cssom/before_pseudo_element.h"
#include "cobalt/cssom/child_combinator.h"
#include "cobalt/cssom/class_selector.h"
#include "cobalt/cssom/compound_selector.h"
#include "cobalt/cssom/css_rule_list.h"
#include "cobalt/cssom/css_style_rule.h"
#include "cobalt/cssom/css_style_sheet.h"
#include "cobalt/cssom/descendant_combinator.h"
#include "cobalt/cssom/empty_pseudo_class.h"
#include "cobalt/cssom/following_sibling_combinator.h"
#include "cobalt/cssom/font_style_value.h"
#include "cobalt/cssom/font_weight_value.h"
#include "cobalt/cssom/id_selector.h"
#include "cobalt/cssom/initial_style.h"
#include "cobalt/cssom/integer_value.h"
#include "cobalt/cssom/keyword_names.h"
#include "cobalt/cssom/keyword_value.h"
#include "cobalt/cssom/length_value.h"
#include "cobalt/cssom/matrix_function.h"
#include "cobalt/cssom/media_query.h"
#include "cobalt/cssom/next_sibling_combinator.h"
#include "cobalt/cssom/number_value.h"
#include "cobalt/cssom/property_list_value.h"
#include "cobalt/cssom/property_names.h"
#include "cobalt/cssom/pseudo_class_names.h"
#include "cobalt/cssom/pseudo_element_names.h"
#include "cobalt/cssom/ratio_value.h"
#include "cobalt/cssom/resolution_value.h"
#include "cobalt/cssom/rgba_color_value.h"
#include "cobalt/cssom/rotate_function.h"
#include "cobalt/cssom/scale_function.h"
#include "cobalt/cssom/string_value.h"
#include "cobalt/cssom/translate_function.h"
#include "cobalt/cssom/type_selector.h"
#include "cobalt/cssom/unicode_range_value.h"
#include "cobalt/cssom/url_value.h"

namespace cobalt {
namespace css_parser {

namespace {

uint32_t ParseHexToken(const TrivialStringPiece& string_piece) {
  char* value_end(const_cast<char*>(string_piece.end));
  uint64 long_integer = std::strtoul(string_piece.begin, &value_end, 16);
  DCHECK_LE(long_integer, std::numeric_limits<uint32_t>::max());
  DCHECK_EQ(value_end, string_piece.end);
  return static_cast<uint32_t>(long_integer);
}

// Ensures that the returned value satisfies the inequality:
// min_value <= value <= max_value.
template <typename Value>
Value ClampToRange(Value min_value, Value max_value, Value value) {
  return std::max(min_value, std::min(value, max_value));
}

}  // namespace

// This class not only hides details of implementation of the parser but also
// provides a low-level API used by semantic actions in grammar.y.
class ParserImpl {
 public:
  ParserImpl(const std::string& input,
             const base::SourceLocation& input_location,
             cssom::CSSParser* css_parser,
             const Parser::OnMessageCallback& on_warning_callback,
             const Parser::OnMessageCallback& on_error_callback,
             Parser::MessageVerbosity message_verbosity);

  cssom::CSSParser* css_parser() { return css_parser_; }

  scoped_refptr<cssom::CSSStyleSheet> ParseStyleSheet();
  scoped_refptr<cssom::CSSRule> ParseRule();
  scoped_refptr<cssom::CSSStyleDeclarationData> ParseStyleDeclarationList();
  scoped_refptr<cssom::CSSFontFaceDeclarationData>
  ParseFontFaceDeclarationList();
  scoped_refptr<cssom::PropertyValue> ParsePropertyValue(
      const std::string& property_name);
  void ParsePropertyIntoDeclarationData(
      const std::string& property_name,
      cssom::CSSDeclarationData* declaration_data);
  scoped_refptr<cssom::MediaQuery> ParseMediaQuery();

  Scanner& scanner() { return scanner_; }

  void set_last_syntax_error_location(
      const YYLTYPE& last_syntax_error_location) {
    last_syntax_error_location_ = last_syntax_error_location;
  }

  void LogWarning(const YYLTYPE& source_location, const std::string& message);
  void LogError(const YYLTYPE& source_location, const std::string& message);

  void set_media_query(const scoped_refptr<cssom::MediaQuery>& media_query) {
    media_query_ = media_query;
  }
  void set_style_sheet(const scoped_refptr<cssom::CSSStyleSheet>& style_sheet) {
    style_sheet_ = style_sheet;
  }
  void set_rule(const scoped_refptr<cssom::CSSRule>& rule) { rule_ = rule; }
  void set_style_declaration_data(
      const scoped_refptr<cssom::CSSStyleDeclarationData>&
          style_declaration_data) {
    style_declaration_data_ = style_declaration_data;
  }
  void set_font_face_declaration_data(
      const scoped_refptr<cssom::CSSFontFaceDeclarationData>&
          font_face_declaration_data) {
    font_face_declaration_data_ = font_face_declaration_data;
  }
  void set_property_value(
      const scoped_refptr<cssom::PropertyValue>& property_value) {
    property_value_ = property_value;
  }

  cssom::CSSDeclarationData* into_declaration_data() {
    return into_declaration_data_;
  }

 private:
  bool Parse();

  std::string FormatMessage(const std::string& message_type,
                            const YYLTYPE& source_location,
                            const std::string& message);

  const std::string input_;
  const base::SourceLocation input_location_;
  const Parser::OnMessageCallback on_warning_callback_;
  const Parser::OnMessageCallback on_error_callback_;
  const Parser::MessageVerbosity message_verbosity_;

  // The CSSParser that created ParserImpl.
  cssom::CSSParser* const css_parser_;

  StringPool string_pool_;
  Scanner scanner_;
  YYLTYPE last_syntax_error_location_;

  // Parsing results, named after entry points.
  // Only one of them may be non-NULL.
  scoped_refptr<cssom::MediaQuery> media_query_;
  scoped_refptr<cssom::CSSStyleSheet> style_sheet_;
  scoped_refptr<cssom::CSSRule> rule_;
  scoped_refptr<cssom::CSSStyleDeclarationData> style_declaration_data_;
  scoped_refptr<cssom::CSSFontFaceDeclarationData> font_face_declaration_data_;
  scoped_refptr<cssom::PropertyValue> property_value_;

  // Acts as the destination declaration data when
  // ParsePropertyIntoDeclarationData() is called.
  cssom::CSSDeclarationData* into_declaration_data_;

  static void IncludeInputWithMessage(const std::string& input,
                                      const Parser::OnMessageCallback& callback,
                                      const std::string& message) {
    callback.Run(message + "\n" + input);
  }

  friend int yyparse(ParserImpl* parser_impl);
};

ParserImpl::ParserImpl(const std::string& input,
                       const base::SourceLocation& input_location,
                       cssom::CSSParser* css_parser,
                       const Parser::OnMessageCallback& on_warning_callback,
                       const Parser::OnMessageCallback& on_error_callback,
                       Parser::MessageVerbosity message_verbosity)
    : input_(input),
      input_location_(input_location),
      on_warning_callback_(on_warning_callback),
      on_error_callback_(on_error_callback),
      message_verbosity_(message_verbosity),
      css_parser_(css_parser),
      scanner_(input_.c_str(), &string_pool_),
      into_declaration_data_(NULL) {}

scoped_refptr<cssom::CSSStyleSheet> ParserImpl::ParseStyleSheet() {
  scanner_.PrependToken(kStyleSheetEntryPointToken);
  return Parse() ? style_sheet_
                 : make_scoped_refptr(new cssom::CSSStyleSheet(css_parser_));
}

scoped_refptr<cssom::CSSRule> ParserImpl::ParseRule() {
  scanner_.PrependToken(kRuleEntryPointToken);
  return Parse() ? rule_ : NULL;
}

scoped_refptr<cssom::CSSStyleDeclarationData>
ParserImpl::ParseStyleDeclarationList() {
  scanner_.PrependToken(kStyleDeclarationListEntryPointToken);
  return Parse() ? style_declaration_data_
                 : make_scoped_refptr(new cssom::CSSStyleDeclarationData());
}

scoped_refptr<cssom::CSSFontFaceDeclarationData>
ParserImpl::ParseFontFaceDeclarationList() {
  scanner_.PrependToken(kFontFaceDeclarationListEntryPointToken);
  return Parse() ? font_face_declaration_data_
                 : make_scoped_refptr(new cssom::CSSFontFaceDeclarationData());
}

scoped_refptr<cssom::PropertyValue> ParserImpl::ParsePropertyValue(
    const std::string& property_name) {
  Token property_name_token;
  bool known_property_name =
      scanner_.DetectPropertyNameToken(property_name, &property_name_token);

  if (!known_property_name) {
    YYLTYPE source_location;
    source_location.first_line = 1;
    source_location.first_column = 1;
    source_location.line_start = input_.c_str();
    LogWarning(source_location, "unsupported property '" + property_name +
                                    "' while parsing property value.");
    return NULL;
  }

  if (input_.empty()) {
    return NULL;
  }

  scanner_.PrependToken(kPropertyValueEntryPointToken);
  scanner_.PrependToken(property_name_token);
  scanner_.PrependToken(':');
  return Parse() ? property_value_ : NULL;
}

void ParserImpl::ParsePropertyIntoDeclarationData(
    const std::string& property_name,
    cssom::CSSDeclarationData* declaration_data) {
  Token property_name_token;
  bool known_property_name =
      scanner_.DetectPropertyNameToken(property_name, &property_name_token);
  if (!known_property_name) {
    YYLTYPE source_location;
    source_location.first_line = 1;
    source_location.first_column = 1;
    source_location.line_start = input_.c_str();
    LogWarning(source_location, "unsupported property '" + property_name +
                                    "' while parsing property value.");
    return;
  }

  scanner_.PrependToken(kPropertyIntoDeclarationDataEntryPointToken);
  scanner_.PrependToken(property_name_token);
  scanner_.PrependToken(':');

  into_declaration_data_ = declaration_data;
  Parse();
}

scoped_refptr<cssom::MediaQuery> ParserImpl::ParseMediaQuery() {
  scanner_.PrependToken(kMediaQueryEntryPointToken);
  return Parse() ? media_query_ : make_scoped_refptr(new cssom::MediaQuery());
}


void ParserImpl::LogWarning(const YYLTYPE& source_location,
                            const std::string& message) {
  on_warning_callback_.Run(FormatMessage("warning", source_location, message));
}

void ParserImpl::LogError(const YYLTYPE& source_location,
                          const std::string& message) {
  on_error_callback_.Run(FormatMessage("error", source_location, message));
}

bool ParserImpl::Parse() {
  // For more information on error codes
  // see http://www.gnu.org/software/bison/manual/html_node/Parser-Function.html
  int error_code(yyparse(this));
  switch (error_code) {
    case 0:
      // Parsed successfully or was able to recover from errors.
      return true;
    case 1:
      // Failed to recover from errors.
      LogError(last_syntax_error_location_, "unrecoverable syntax error");
      return false;
    default:
      NOTREACHED();
      return false;
  }
}

namespace {

// This function will return a string equal to the input string except ended
// at a newline or a null character.
std::string GetLineString(const char* line_start) {
  const char* line_end = line_start;
  while (*line_end != '\n' && *line_end != '\0') {
    ++line_end;
  }

  return std::string(line_start, static_cast<size_t>(line_end - line_start));
}

}  // namespace

std::string ParserImpl::FormatMessage(const std::string& message_type,
                                      const YYLTYPE& source_location,
                                      const std::string& message) {
  // Adjust source location for CSS embedded in HTML.
  int line_number = source_location.first_line;
  int column_number = source_location.first_column;
  base::AdjustForStartLocation(input_location_.line_number,
                               input_location_.column_number, &line_number,
                               &column_number);

  std::stringstream message_stream;

  // 1st line: location and message.
  message_stream << input_location_.file_path << ":" << line_number << ":"
                 << column_number << ": " << message_type << ": " << message;

  if (message_verbosity_ == Parser::kVerbose) {
    // 2nd line: the content of the line, with a maximum of kLineMax characters
    // around the location.
    //
    const int kLineMax = 80;
    const std::string line = GetLineString(source_location.line_start);
    const int line_length = static_cast<int>(line.length());
    // The range of index of the substr is [substr_start, substr_end).
    // Shift the range left and right, and trim to the correct length, to make
    // it fit in [0, line.length()).
    int substr_start = column_number - kLineMax / 2;
    int substr_end = column_number + kLineMax / 2;
    if (substr_end > line_length) {
      int delta = substr_end - line_length;
      substr_start -= delta;
      substr_end -= delta;
    }
    if (substr_start < 0) {
      int delta = -substr_start;
      substr_start += delta;
      substr_end += delta;
    }
    if (substr_end > line_length) {
      substr_end = line_length;
    }
    // Preamble and postamble are printed before and after the sub string.
    const std::string preamble = substr_start == 0 ? "" : "...";
    const std::string postamble = substr_end == line_length ? "" : "...";

    message_stream << std::endl << preamble
                   << line.substr(
                          static_cast<size_t>(substr_start),
                          static_cast<size_t>(substr_end - substr_start))
                   << postamble;

    // 3rd line: a '^' arrow to indicate the exact column in the line.
    //
    message_stream << std::endl
                   << std::string(
                          preamble.length() + column_number - substr_start - 1,
                          ' ') << '^';
  }

  return message_stream.str();
}

// This function is only used to record a location of unrecoverable
// syntax error. Most of error reporting is implemented in semantic actions
// in the grammar.
inline void yyerror(YYLTYPE* source_location, ParserImpl* parser_impl,
                    const char* /*message*/) {
  parser_impl->set_last_syntax_error_location(*source_location);
}

// TODO(***REMOVED***): Revisit after upgrading to Bison 3.
#if defined(COBALT_WIN)
#pragma warning(push)
#pragma warning(disable : 4242)  // possible loss of data
#pragma warning(disable : 4244)  // possible loss of data
#pragma warning(disable : 4365)  // signed/unsigned mismatch
#pragma warning(disable : 4701)  // potentially uninitialized local variable
#pragma warning(disable : 4702)  // unreachable code
#endif

#if defined(COBALT_LINUX)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#endif

// A header generated by Bison must be included inside our namespace
// to avoid global namespace pollution.
#include "cobalt/css_parser/grammar_impl_generated.h"

#if defined(COBALT_LINUX)
#pragma clang diagnostic pop
#endif

#if defined(COBALT_WIN)
#pragma warning(pop)
#endif

namespace {

void LogWarning(const std::string& message) { LOG(WARNING) << message; }

void LogError(const std::string& message) { LOG(ERROR) << message; }

}  // namespace

scoped_ptr<Parser> Parser::Create() {
  return make_scoped_ptr(new Parser(base::Bind(&LogWarning),
                                    base::Bind(&LogError), Parser::kVerbose));
}

Parser::Parser(const OnMessageCallback& on_warning_callback,
               const OnMessageCallback& on_error_callback,
               MessageVerbosity message_verbosity)
    : on_warning_callback_(on_warning_callback),
      on_error_callback_(on_error_callback),
      message_verbosity_(message_verbosity) {}

Parser::~Parser() {}

scoped_refptr<cssom::CSSStyleSheet> Parser::ParseStyleSheet(
    const std::string& input, const base::SourceLocation& input_location) {
  ParserImpl parser_impl(input, input_location, this, on_warning_callback_,
                         on_error_callback_, message_verbosity_);
  return parser_impl.ParseStyleSheet();
}

scoped_refptr<cssom::CSSRule> Parser::ParseRule(
    const std::string& input, const base::SourceLocation& input_location) {
  ParserImpl parser_impl(input, input_location, this, on_warning_callback_,
                         on_error_callback_, message_verbosity_);
  return parser_impl.ParseRule();
}

scoped_refptr<cssom::CSSStyleDeclarationData> Parser::ParseStyleDeclarationList(
    const std::string& input, const base::SourceLocation& input_location) {
  ParserImpl parser_impl(input, input_location, this, on_warning_callback_,
                         on_error_callback_, message_verbosity_);
  return parser_impl.ParseStyleDeclarationList();
}

scoped_refptr<cssom::CSSFontFaceDeclarationData>
Parser::ParseFontFaceDeclarationList(
    const std::string& input, const base::SourceLocation& input_location) {
  ParserImpl parser_impl(input, input_location, this, on_warning_callback_,
                         on_error_callback_, message_verbosity_);
  return parser_impl.ParseFontFaceDeclarationList();
}

scoped_refptr<cssom::PropertyValue> Parser::ParsePropertyValue(
    const std::string& property_name, const std::string& property_value,
    const base::SourceLocation& property_location) {
  ParserImpl parser_impl(property_value, property_location, this,
                         on_warning_callback_, on_error_callback_,
                         message_verbosity_);
  return parser_impl.ParsePropertyValue(property_name);
}

void Parser::ParsePropertyIntoDeclarationData(
    const std::string& property_name, const std::string& property_value,
    const base::SourceLocation& property_location,
    cssom::CSSDeclarationData* declaration_data) {
  ParserImpl parser_impl(property_value, property_location, this,
                         on_warning_callback_, on_error_callback_,
                         message_verbosity_);
  return parser_impl.ParsePropertyIntoDeclarationData(property_name,
                                                      declaration_data);
}

scoped_refptr<cssom::MediaQuery> Parser::ParseMediaQuery(
    const std::string& input, const base::SourceLocation& input_location) {
  ParserImpl parser_impl(input, input_location, this, on_warning_callback_,
                         on_error_callback_, message_verbosity_);
  return parser_impl.ParseMediaQuery();
}

}  // namespace css_parser
}  // namespace cobalt
