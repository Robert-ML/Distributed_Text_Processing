#include "./master.hpp"

#include "mpi.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <pthread.h>
#include <queue>
#include <sstream>
#include <stdlib.h>
#include <string>

Master& Master::get_instance() {
  static Master instance;
  return instance;
}

Master::Master() : rank(0), file_in(nullptr), genres_order(std::queue<int>()) {
  pthread_mutex_init(&mutex_queue, NULL);
  cond_queue = PTHREAD_COND_INITIALIZER;
}

void Master::run(const int rank, const int no_tasks, char* file_in) {
  this->rank = rank;
  this->file_in = file_in;

  {
    std::string temp = std::string(file_in);
    std::string::size_type i = temp.rfind('.', temp.length());
    if (i != std::string::npos) {
      temp.replace(i + 1, 3, "out");
    }
    fout.open(temp);
    if (fout.fail()) {
      std::cerr << "Could not open the output file with the mane \"" << temp << "\" | err in " << __FILE__ << __func__ << __LINE__ << '\n';
      std::exit(-1);
    }
  }

  std::unique_ptr<pthread_t> threads((pthread_t*)malloc(no_tasks * sizeof(pthread_t)));
  std::unique_ptr<int> args(new int[no_tasks]);

  // starting the threads
  int ret;
  for (int i = 0; i < no_tasks; ++i) {
    args.get()[i] = i;
    ret = pthread_create(&(threads.get()[i]), NULL, Master::handle_worker, (void*) &(args.get()[i]));
    if (ret != 0) {
      std::cerr << "Error at creating thread " << i << " | err in " << __FILE__ << __func__ << __LINE__ << '\n';
      std::exit(-1);
    }
  }

  // reding the order of the genres
  this->create_genre_order();

  // closing the threads
  for (int i = 0; i < no_tasks; ++i) {
    ret = pthread_join(threads.get()[i], NULL);
    if (ret != 0) {
      std::cerr << "Error at joining thread " << i << " | err in " << __FILE__ << __func__ << __LINE__ << '\n';
      std::exit(-1);
    }
  }
  fout.close();
}

void* Master::handle_worker(void* arg) {
  int id = *((int*) arg);
  int worker_rank = id + 1;
  
  std::ifstream fin(Master::get_instance().file_in);
  if (fin.fail()) {
    std::cerr << "Could not open the file in thread " << id << " | err in " << __FILE__ << __func__ << __LINE__ << '\n';
    std::exit(-1);
  }

  // printing queue
  std::queue<std::string> pq;

  int buff_size = ct::buff_init_size;
  std::unique_ptr<char> buff((char*) malloc(buff_size * sizeof(char)));

  std::string line;
  std::stringstream paragraph;
  int no_lines;
  bool first_time = true;

  while (fin.eof() == false) {
    // get the next paragraph
    if (std::getline(fin, line), line == "" || fin.eof() == true) {
      break;
    }
    
    no_lines = 0;
    int target = Master::get_genre_rank(line);
    bool good_topic = (target == worker_rank); // skip paragraph if it's from another topic
    if (good_topic) {
      while (std::getline(fin, line) && line != "") {
        ++no_lines;
        paragraph << line << '\n';
      }
    } else {
      while (std::getline(fin, line), fin.eof() == false && line != "") { }
      continue;
    }

    if (first_time == false) {
      int needed_buff_size = Master::get_instance().master_recv(worker_rank, buff, buff_size);
      // try to print to file
      Master::get_instance().print_or_store(worker_rank, std::string(buff.get(), needed_buff_size), pq);
    } else {
      first_time = false;
    }

    std::string payload_s(paragraph.str());
    paragraph.str(std::string("")); // clear the stream
    payload_s.pop_back(); // remove the last
    Master::get_instance().master_send(payload_s, no_lines, worker_rank);
  }

  // there was at least one more send then recv so we must recv the data
  if (first_time == false) {
    int needed_buff_size = Master::get_instance().master_recv(worker_rank, buff, buff_size);
    Master::get_instance().print_or_store(worker_rank, std::string(buff.get(), needed_buff_size), pq);
  }

  // tell the Worker to close
  int end_work = ct::ended_work_code;
  MPI_Send(&end_work, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD);

  // print to file until printing queue is empty
  Master::get_instance().clear_queue(worker_rank, pq);

  pthread_exit(NULL);
}

void Master::master_send(const std::string& payload, const int no_lines, const int dest) {
    // send number of lines and paragrapf to worker
    MPI_Send((void*)&no_lines, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    MPI_Send((void*)payload.c_str(), payload.length(), MPI_CHAR, dest, dest, MPI_COMM_WORLD);
}

int Master::master_recv(const int worker_rank, std::unique_ptr<char>& buff, int buff_size) {
  MPI_Status status;
  int needed_buff_size = 0;
  MPI_Probe(worker_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  MPI_Get_count(&status, MPI_CHAR, &needed_buff_size);
  if (needed_buff_size > buff_size) {
    buff_size = std::max(needed_buff_size, (int)1.5 * buff_size);
    buff.reset((char*) malloc(buff_size * sizeof(char)));
  }
  MPI_Recv(buff.get(), needed_buff_size, MPI_CHAR, worker_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

  return needed_buff_size;
}

void Master::print_or_store(const int worker_rank, const std::string& processed, std::queue<std::string>& pq) {  
  pthread_mutex_lock(&mutex_queue);

  if (genres_order.size() == 0) {
    pq.push(processed);
  } else {
    std::string to_print;
    bool print_stored = false;
    // check who is up to be printed
    if (pq.size() == 0) {
      to_print = processed;
    } else {
      print_stored = true;
      to_print = pq.front();
      pq.push(processed);
    }
    
    // there is no element
    if (genres_order.front() == worker_rank) {
      genres_order.pop();
      // this worker's turn to print to file
      fout << to_print << '\n';
      if (print_stored == true) {
        pq.pop();
      }
    } else if (print_stored == false) {
      pq.push(processed);
    }
  }

  pthread_mutex_unlock(&mutex_queue);  
}

void Master::clear_queue(const int worker_rank, std::queue<std::string>& pq) {
  while (pq.size() != 0) {
    pthread_mutex_lock(&mutex_queue);
    if (this->genres_order.size() == 0) {
      std::cerr << "Error: queue genres_order is empty but threads still have their queue's non-empty\n";
    } else if (this->genres_order.front() == worker_rank) {
      this->genres_order.pop();
      fout << pq.front() << '\n';
      pq.pop();
      // not working for some reason
      // pthread_cond_broadcast(&cond_queue);
    } else {
      // not working for some reason
      // pthread_cond_wait(&cond_queue, &mutex_queue);
    }
    pthread_mutex_unlock(&mutex_queue);
  }
}

void Master::create_genre_order() {
  std::ifstream fin(Master::get_instance().file_in);
  if (fin.fail()) {
    std::cerr << "Could not open the file | err in " << __FILE__ << __func__ << __LINE__ << '\n';
    std::exit(-1);
  }

  std::string line;
  while (fin.eof() == false) {
    if (std::getline(fin, line), line == "" || fin.eof() == true) {
      break;
    }
    int target = Master::get_genre_rank(line);
    pthread_mutex_lock(&this->mutex_queue);
    this->genres_order.push(target);
    pthread_mutex_unlock(&this->mutex_queue);
    while (std::getline(fin, line), fin.eof() == false && line != "") { }
  }
}

int Master::get_genre_rank(const std::string& s) {
  if (s == ct::comedy_tag) {
    return ct::comedy_rank;
  } else if (s == ct::fantasy_tag) {
    return ct::fantasy_rank;
  } else if (s == ct::horror_tag) {
    return ct::horror_rank;
  } else if (s == ct::science_fiction_tag) {
    return ct::science_fiction_rank;
  } else {
    std::cerr << "Warning: genre rank not found from \"" << s << "\" , returning -1 | warn in " << __FILE__ << __func__ << __LINE__ << '\n';
    return -1;
  }
}