#ifndef MASTER_MASTER_H
#define MASTER_MASTER_H

#include "../utils/constants.hpp"

#include <fstream>
#include <memory>
#include <queue>

class Master final {
  int rank;
  char* file_in;
  pthread_cond_t cond_queue;
  pthread_mutex_t mutex_queue;
  std::queue<int> genres_order;
  std::ofstream fout;

  Master();

 public:
  static Master& get_instance();
  Master& operator=(const Master& other) = delete;
  Master(const Master& other) = delete;

  void run(const int rank, const int no_tasks, char* file_in);

 private:
  static void* handle_worker(void* arg);
  static int get_genre_rank(const std::string& s);

  void master_send(const std::string& payload, const int no_lines, const int dest);
  int master_recv(const int worker_rank, std::unique_ptr<char>& buff, int buff_size);

  void create_genre_order();
  void print_or_store(const int worker_rank, const std::string& processed, std::queue<std::string>& pq);
  void clear_queue(const int worker_rank, std::queue<std::string>& pq);
};

#endif