#ifndef GENRES_SCIENCE_FICTION_SCIENCE_FICTION_HPP
#define GENRES_SCIENCE_FICTION_SCIENCE_FICTION_HPP

#include "../genre.hpp"

#include <string>

class ScienceFiction : public Genre {
 public:
  ScienceFiction(const int rank) : Genre(rank) { }

 private:
  std::string get_genre_name() override;

  std::string process(const std::string& val) override;
};

#endif // GENRES_SCIENCE_FICTION_SCIENCE_FICTION_HPP