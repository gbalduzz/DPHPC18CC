#pragma once

#include <mpi.h>

#ifdef USE_FOMPI
#include "fompi.h"

#define MMPI_WIN foMPI_Win
#define MMPI_WIN_FENCE foMPI_Win_fence
#define MMPI_GET foMPI_Get
#define MMPI_PUT foMPI_Put
#define MMPI_INIT foMPI_Init
#define MMPI_FINALIZE foMPI_Finalize
#define MMPI_WIN_FREE foMPI_Win_free
#define MMPI_WIN_ALLOCATE foMPI_Win_allocate
#define MMPI_ALLOC_MEM foMPI_Alloc_mem
#define MMPI_WIN_CREATE foMPI_Win_create
#define MMPI_WIN_FLUSH foMPI_Win_flush
#define MMPI_WIN_LOCK foMPI_Win_lock
#define MMPI_WIN_UNLOCK foMPI_Win_unlock
#define MMPI_WIN_LOCK_ALL foMPI_Win_lock_all
#define MMPI_WIN_UNLOCK_ALL foMPI_Win_unlock_all
#define MMPI_WIN_GET_GROUP foMPI_Win_get_group
#define MMPI_INIT_THREAD foMPI_Init_thread

#else // USE_FOMPI

#define MMPI_WIN MPI_Win
#define MMPI_WIN_FENCE MPI_Win_fence
#define MMPI_GET MPI_Get
#define MMPI_PUT MPI_Put
#define MMPI_COMPARE_AND_SWAP MPI_Compare_and_swap
#define MMPI_INIT MPI_Init
#define MMPI_FINALIZE MPI_Finalize
#define MMPI_WIN_FREE MPI_Win_free
#define MMPI_WIN_ALLOCATE MPI_Win_allocate
#define MMPI_ALLOC_MEM MPI_Alloc_mem
#define MMPI_WIN_CREATE MPI_Win_create
#define MMPI_WIN_FLUSH MPI_Win_flush
#define MMPI_WIN_LOCK MPI_Win_lock
#define MMPI_WIN_UNLOCK MPI_Win_unlock
#define MMPI_WIN_LOCK_ALL MPI_Win_lock_all
#define MMPI_WIN_UNLOCK_ALL MPI_Win_unlock_all
#define MMPI_WIN_GET_GROUP MPI_Win_get_group
#define MMPI_INIT_THREAD MPI_Init_thread

#endif // USE_FOMPI
