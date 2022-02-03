#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

auto read_file(const char *path) -> std::string;
void minify_file(const char *, const char *);
auto minify(const std::string &css) -> std::string;

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " CSSFILE OUTPUTFILE\n";
    return 1;
  }
  const char *filepath = argv[1];
  const char *outpath  = argv[2];

  // TODO: add support for optional OUTFILE, where output filename is derieved from the css filename.
  // Should print an error and exit if a file with the derieved name already exists.

  minify_file(filepath, outpath);
  return 0;
}

auto read_file(const char *path) -> std::string {
  constexpr auto read_size = std::size_t(4096); // <- Exactly what a typical modern C++ programmer would do.
  auto stream = std::ifstream(path);
  stream.exceptions(std::ios_base::badbit);

  auto out = std::string();
  auto buf = std::string(read_size, '\0');
  while (stream.read(&buf[0], read_size)) {
    out.append(buf, 0, stream.gcount());
  }
  out.append(buf, 0, stream.gcount());

  return out;
}

void minify_file(const char *cssfilepath, const char *outfilepath) {
  auto css = read_file(cssfilepath);
  auto minicss = minify(css);
  auto out = std::ofstream(outfilepath);
  out << minicss;
}

auto minify(const std::string& css) -> std::string {
  auto minicss = std::string();
  minicss.reserve(css.size());

  bool last_was_space = true; // 'true' to deal with the whitespaces at the very beginning of the file.
  char c;
  try {
      for (size_t i = 0; i < css.size(); ++i) {
	c = css.at(i);
	switch (c) {
	case '\t':
	case ' ' :
	  {
	    // Replace a sequence of whitespaces with a single space character.
	    // Note: make sure to set last_was_space to false, everywhere else where a not whitespace
	    // character has been appended to 'minicss'.
	    if (!last_was_space) {
	      minicss += ' ';
	      last_was_space = true;
	    }
	  } break;
	case '\n': break; // no newlines allowed!
	case ':':
	case ';':
	case ',':
	case '>':
	case '{':
	case '}':
	  {
	    // Make sure that no whitespace is present before the above characters.
	    if (minicss.back() == ' ') minicss.back() = c;
	    else minicss += c;
	    // Consume all whitespace after the above characters.
	    while (i < css.size()-1 && std::isspace(css.at(i+1))) i++;
	    last_was_space = false;
	  } break;
	case '\'':
	case '"' :
	  {
	    // Preseve the whitespaces in strings.
	    // TODO: this fails if there is an escaped quote between the string. (Is it allowed?)
	    minicss += c;
	    while (i < css.size()-1 && css.at(++i) != c) minicss += css.at(i);
	    minicss += c;
	    last_was_space = false;
	  } break;
	case '/' :
	  {
	    // Consume the comment.
	    if (css.at(++i) == '*') {
	      while (!(css.at(++i) == '*' && css.at(++i) == '/')) {}
	    }
	    else minicss += '/';
	  } break;

	default: minicss += c; last_was_space = false;
	}
      }
  } catch (std::out_of_range &e) {
    std::cerr << "Unexpected end of file reached, possible syntax error.\n";
    std::exit(1);
  }

  minicss.shrink_to_fit();
  return minicss;
}
