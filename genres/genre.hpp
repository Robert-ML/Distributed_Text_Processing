#ifndef GENRES_GENRE_H
#define GENRES_GENRE_H

#include "../utils/constants.hpp"

#include <pthread.h> // I couldn't get C++20 to work in my environment so I could have std::barrier :((

#include <atomic>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

class Genre {
  typedef struct thread_info {
    // thread dependent
    int id;
    // for thread syncronization
    pthread_barrier_t& b;
    volatile bool& exit;
    // shared information
    Genre& context;
    std::atomic<std::size_t>& index;
    std::vector<std::string>& payload;
    std::vector<std::string>& processed;
    
  } thread_info;

  const int rank;
  int buff_size;
  std::unique_ptr<char> buff;

 public:
  Genre(const int rank);

  virtual void run();

 protected:
  virtual std::string process(const std::string& val) = 0;

 private:
  void send(const std::string& data);
  std::tuple<std::string, int> recv();
  
  virtual std::string get_genre_name() = 0;

  static void communicator(thread_info& arg);
  static void divide_work(const std::string& paragraph, thread_info& arg);

  static void working_thread(thread_info& arg);
};

#endif // GENRES_GENRE_H