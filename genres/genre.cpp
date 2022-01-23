#include "./genre.hpp"

#include "mpi.h"
#include "../utils/constants.hpp"

#include <pthread.h>

#include <atomic>
#include <numeric>
#include <sstream>
#include <thread>
#include <vector>

Genre::Genre(const int rank) : rank(rank),
                               buff_size(ct::buff_init_size),
                               buff((char*) malloc(buff_size * sizeof(char))) { }

void Genre::run() {
  auto th_to_start = ct::P;
  std::vector<std::thread> th_v;
  std::atomic<std::size_t> index(0);
  std::vector<std::string> payload;
  std::vector<std::string> processed;
  pthread_barrier_t b;
  pthread_barrier_init(&b, NULL, ct::P);
  bool end_work = false;
  std::vector<thread_info> args(th_to_start, thread_info { 0, b, end_work, *this, index, payload, processed });
  
  // start comunicator
  args[0].id = ct::communicator_id;
  th_v.emplace_back(std::thread(Genre::communicator, std::ref(args[0])));
  
  // start the working threads
  for (auto i = 0; i < th_to_start - 1; ++i) {
    args[1 + i].id = ct::worker_start_id + i;
    th_v.emplace_back(std::thread(Genre::working_thread, std::ref(args[1 + i])));
  }

  for (auto& x : th_v) {
    x.join();
  }

  pthread_barrier_destroy(&b);
}

void Genre::communicator(thread_info& arg) {
  while (true) {
    std::string paragraph;
    int no_lines;
    {
      auto temp = arg.context.recv();
      paragraph = std::get<0>(temp);
      no_lines = std::get<1>(temp);
    }
    if (no_lines == ct::ended_work_code) {
      arg.exit = true;
      pthread_barrier_wait(&arg.b);
      break;
    }

    Genre::divide_work(paragraph, arg);

    pthread_barrier_wait(&arg.b);
    
    // wait for the workers to finish
    pthread_barrier_wait(&arg.b);
    
    arg.context.send(arg.context.get_genre_name()
      + std::accumulate(arg.processed.begin(), arg.processed.end(), std::string(""))
      + std::string("\n"));
      
    // clean stuff
    arg.index = 0;
    arg.payload.erase(arg.payload.begin(), arg.payload.end());
    arg.processed.erase(arg.processed.begin(), arg.processed.end());
  }
}

void Genre::divide_work(const std::string& paragraph, thread_info& arg) {
  std::string line;
  std::istringstream ss(paragraph);
  std::ostringstream ssb;

  while(std::getline(ss, line, ct::line_delimiter)) {
    ssb << '\n' << line;
    for (int i = 1; i < ct::thread_max_lines && std::getline(ss, line, ct::line_delimiter); ++i) {
      ssb << '\n' << line;
    }
    arg.payload.emplace_back(std::move(ssb.str()));
    ssb.str(std::string(""));
  }

  arg.processed.resize(arg.payload.size());
}

void Genre::working_thread(thread_info& arg) {
  while (true) {
    pthread_barrier_wait(&arg.b);
    if (arg.exit == true) {
      return;
    }
    
    while (true) {
      auto index = arg.index.fetch_add(1, std::memory_order_relaxed);
      if (index >= arg.payload.size()) {
        break;
      }
      arg.processed[index] = arg.context.process(arg.payload[index]);
    }

    pthread_barrier_wait(&arg.b);
    // comm sends data from here
  }
}

void Genre::send(const std::string& data) {
  MPI_Send((void*)data.c_str(), data.length(), MPI_CHAR, ct::master_rank, rank, MPI_COMM_WORLD);
}

std::tuple<std::string, int> Genre::recv() {
  MPI_Status status;

  // receive the no of lines
  int no_lines = 0;
  MPI_Recv(&no_lines, 1, MPI_INT, ct::master_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  if (no_lines == ct::ended_work_code) {
    return std::make_tuple("", ct::ended_work_code);
  }

  int needed_buff_size = 0;
  // get info about the buffer
  MPI_Probe(ct::master_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  MPI_Get_count(&status, MPI_CHAR, &needed_buff_size);
  if (needed_buff_size > buff_size) {
    buff_size = std::max(needed_buff_size, (int)1.5 * buff_size);
    buff.reset((char*) malloc(buff_size * sizeof(char)));
  }
  MPI_Recv(buff.get(), needed_buff_size, MPI_CHAR, ct::master_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

  return std::make_tuple(std::string(buff.get(), needed_buff_size), no_lines);
}