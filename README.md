_This project has been created as part of the 42 curriculum by marberge._

<div align="center">
<br>
  <img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTQPzuYKu7n0cWUYa5Kbg0_LrlEQAIURWeo9A&s" alt="42 Logo" width="400" />

  <br>
</div>

# Codexion

<div align="center">
	<img src="https://img.shields.io/badge/Project-000000?style=for-the-badge&logo=42&logoColor=white" alt="42" />
	<img src="https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=C&logoColor=white" alt="C" />
	<img src="https://img.shields.io/badge/Score-0/100-orange?style=for-the-badge&logo=C&logoColor=white" alt="C" />
	<br>
	<img src="https://img.shields.io/badge/Multithreading-darkviolet?style=for-the-badge" alt="Multithreading" />
	<img src="https://img.shields.io/badge/Concurrency-darkviolet?style=for-the-badge" alt="POSIX Threads" />
	<img src="https://img.shields.io/badge/POSIX__Threads-darkviolet?style=for-the-badge" alt="POSIX Threads" />
	<img src="https://img.shields.io/badge/Mutexes-darkviolet?style=for-the-badge" alt="Mutexes" />
	<img src="https://img.shields.io/badge/Min--Heap-darkviolet?style=for-the-badge" alt="Min-Heap" />
	<img src="https://img.shields.io/badge/EDF_%26_FIFO-darkviolet?style=for-the-badge" alt="Scheduling" />
</div>

## I. Description

Codexion is an advanced system programming project from the 42 curriculum. It introduces multithreading, concurrency, and process scheduling concepts. The goal is to simulate a group of coders who need shared hardware tokens (dongles) to work (compile, debug, and refactor). If a coder waits too long for a dongle, they burn out, ending the simulation. 

To manage resource allocation, the project implements a custom priority queue (Min-Heap) that acts as an intelligent scheduler, arbitrating dongle distribution using either a First-In-First-Out (FIFO) or an Earliest Deadline First (EDF) algorithm.

## II. Instructions

To compile the project, run the Makefile at the root of the repository.

```bash
make
```

Run the executable with the following arguments:

```bash
./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler_policy]
```

Example to run a simulation with 5 coders using the EDF scheduler:

```bash
./codexion 5 650 200 200 200 7 10 edf
```

## III. Resources

### Threads :

- [Les PThreads - POSIX Threads - Paris Descartes](https://helios2.mi.parisdescartes.fr/~soto/dokuwiki/lib/exe/fetch.php?media=teaching%3Agestionthreads.pdf)    
- [Threads POSIX (1) Création et gestion](https://perso.ens-lyon.fr/michael.rao/ASR2/threads.pdf)    
- [POSIX threads - Bien programmer en langage C](https://www.bien-programmer.fr/pthreads.htm)
- [POSIX threads - Emmanuel Delahaye](https://emmanuel-delahaye.developpez.com/tutoriels/c/posix-threads-c/)
- [programmation multitâche en C avec Pthreads](https://franckh.developpez.com/tutoriels/posix/pthreads/)

### Queue :

- [File (structure de données)](https://fr.wikipedia.org/wiki/File_(structure_de_donn%C3%A9es))

### Heap:

- [Heap in C](https://www.geeksforgeeks.org/c/heap-in-c/)

- [Apprendre à programmer les arbres en langage C](https://chgi.developpez.com/arbre/tas/)

### EDF algorithm:

- https://www.youtube.com/watch?v=33PyyzqAd6Y

### AI

Artificial Intelligence (Gemini) was utilized as an interactive tutor to solidify concepts related to Min-Heap data structures, understand thread rollback mechanisms upon malloc failures, and debug multithreading anomalies via Valgrind/Helgrind outputs.
It was also used to help building this readme.

## IV. Additional content

### Blocking cases handled
- **Deadlock prevention and Coffman’s conditions:** Circular wait is strictly prevented by establishing a global resource ordering. Coders always attempt to acquire the dongle with the lowest ID first, breaking the symmetry that causes deadlocks.

- **Starvation prevention:** The priority queue ensures no coder is left behind. In FIFO mode, the oldest requests are served first. In EDF mode, the coder closest to burning out is placed at the top of the queue, with a deterministic tie-breaker based on coder IDs.

- **Cooldown handling:** Each dongle tracks an `available_at` timestamp. Threads actively yield the CPU if a dongle is locked or in cooldown, preventing early acquisition.

- **Precise burnout detection:** A dedicated, independent `monitor` thread constantly verifies the status of all coders with a 1ms resolution (`usleep(1000)`). The absolute starting time (T0) is synchronized perfectly before thread creation to ensure mathematical accuracy.

- **Log serialization:** Terminal output is protected by a dedicated `write_mutex` to prevent scrambled or interleaved messages.

### Thread synchronization mechanisms
- **Threading primitives:** The project heavily relies on `pthread_mutex_t` to protect shared resources (dongles, coder states, and simulation status). While `pthread_cond_t` is a standard approach, this implementation uses a highly controlled active polling loop with `usleep` for dongle acquisition. This design choice simplifies the complex interaction between the Min-Heap priority queue and the threads, allowing for dynamic wait queues without complex timed-wait structures.

- **Shared resources:** The `sim->is_running` flag acts as the global kill switch and is protected by `sim_mutex`. Dongles are protected by their own individual mutexes, holding their respective waiting queues.

- **Race condition prevention:** Data races are eliminated by strict lock ordering. For instance, when the monitor detects a burnout, it locks the `write_mutex` first, then the `sim_mutex`. The coder threads follow this exact same order when logging their actions, completely removing the risk of cross-locking.

- **Thread-safe communication:** The monitor and coder threads never communicate directly. They read and update shared state variables (`last_compile_start`, `compiles_done`) safely wrapped in individual `state_mutexes`. Furthermore, dynamic memory allocation failures during thread creation trigger a clean rollback mechanism, joining existing threads before exiting to prevent memory leaks.