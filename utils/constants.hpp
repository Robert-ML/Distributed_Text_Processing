#ifndef UTILS_CONSTANTS_H
#define UTILS_CONSTANTS_H

#include <string>
#include <unistd.h>

namespace ct {
  const int no_mpi_instances = 5;
  const int needed_narg = 1;
  const int ended_work_code = -1;
  // ranks
  const int master_rank = 0;
  // genres ranks
  const int comedy_rank = 1;
  const int fantasy_rank = 2;
  const int horror_rank = 3;
  const int science_fiction_rank = 4;

  // genres tags
  const std::string comedy_tag = "comedy";
  const std::string fantasy_tag = "fantasy";
  const std::string horror_tag = "horror";
  const std::string science_fiction_tag = "science-fiction";

  // limits
  static long cores = sysconf(_SC_NPROCESSORS_CONF);
  static long minimum_thread_no = 2;
  static long P = std::max(cores, minimum_thread_no);
  const char line_delimiter = '\n';
  const int thread_max_lines = 20;
  const int buff_init_size = 10;

  // thread id's
  const int worker_start_id = 0;
  const int communicator_id = worker_start_id - 1;
};

#endif // UTILS_CONSTANTS_H