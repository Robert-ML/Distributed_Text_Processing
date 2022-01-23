#ifndef GENRES_COMEDY_COMEDY_HPP
#define GENRES_COMEDY_COMEDY_HPP

#include "../genre.hpp"

#include <string>

class Comedy : public Genre {
 public:
  Comedy(const int rank) : Genre(rank) { }

 private:
  std::string get_genre_name() override;

  std::string process(const std::string& val) override;
};

#endif // GENRES_COMEDY_COMEDY_HPP