#include "mpi.h"

#include <iostream>

#include "./master/master.hpp"
#include"./genres/comedy/comedy.hpp"
#include"./genres/fantasy/fantasy.hpp"
#include"./genres/horror/horror.hpp"
#include"./genres/science_fiction/science_fiction.hpp"
#include "./utils/constants.hpp"

void no_arg(const int argc, char* args[]);

int main(int argc, char* args[]) {
  no_arg(argc, args);
  
  int no_tasks, rank, len;
  no_tasks = ct::no_mpi_instances;
  char hostname[MPI_MAX_PROCESSOR_NAME];
  MPI_Init_thread(&argc, &args, MPI_THREAD_MULTIPLE, &no_tasks);
  MPI_Comm_size(MPI_COMM_WORLD, &no_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Get_processor_name(hostname, &len);

  switch (rank) {
  case ct::master_rank:
    Master::get_instance().run(rank, no_tasks - 1, args[1]);
    break;

  case ct::comedy_rank:
    Comedy(rank).run();
    break;
  
  case ct::fantasy_rank:
    Fantasy(rank).run();
    break;

  case ct::horror_rank:
    Horror(rank).run();
    break;

  case ct::science_fiction_rank:
    ScienceFiction(rank).run();
    break;

  default:
    std::cerr << "Warning: rank " << rank << " case not covvered | warn in " << __FILE__ << __func__ << __LINE__ << '\n';
    break;
  }
  MPI_Finalize();
  return 0;
}

void no_arg(const int argc, char* args[]) {
  if (argc < 1 + ct::needed_narg) {
    std::cout << "Error: too few arguments.\nRunning command: ./" << args[0] << " <input_file_name>\n";
    std::exit(-1); 
  }
}
