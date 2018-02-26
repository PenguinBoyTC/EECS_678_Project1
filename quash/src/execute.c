/**
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */

#include "execute.h"
#include "deque.h"
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "quash.h"

// Remove this and all expansion calls to it
/**
 * @brief Note calls to any function that requires implementation
 */
#define IMPLEMENT_ME()                                                  \
  fprintf(stderr, "IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)
IMPLEMENT_DEQUE_STRUCT(Pid_Q, pid_t);
IMPLEMENT_DEQUE(Pid_Q, pid_t);
PROTOTYPE_DEQUE(Pid_Q, pid_t);

int pipe1[2];
//decide if run_script() function ran once
bool isJob_run = false;
//Pid_Q Pids;
typedef struct job_t{
  Pid_Q pids;//name
  int jobId;
  char* cmd;
} job_t;

IMPLEMENT_DEQUE_STRUCT(Job_Q, job_t);//structure name is Job_Q, type is job_t
IMPLEMENT_DEQUE(Job_Q, job_t);
PROTOTYPE_DEQUE(Job_Q, job_t);

Job_Q jobs; //jobs = NULL

//Pid_Q Pids;
//Job_Q Job;

/***************************************************************************
 * Interface Functions
 ***************************************************************************/

// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
  // TODO: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();

  // Change this to true if necessary
  *should_free = true;
	char *pwd;
	pwd = getcwd(NULL, 0);
	//pwd = get_current_dir_name();
  return(pwd);
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {
  // TODO: Lookup environment variables. This is required for parser to be able
  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();

  // TODO: Remove warning silencers
  return getenv(env_var);
}

// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  //IMPLEMENT_ME();
  bool iscompleted =false;
  if(is_empty_Job_Q(&jobs))
  {
	  return;
  }
  else
  {
	  for(int i = 0; i < length_Job_Q(&jobs); i++)
	  {
		job_t tempjob = pop_front_Job_Q(&jobs);
		pid_t temppid;
		for(int j = 0; j< length_Pid_Q(&tempjob.pids); j++)
		{
		  int status;
		  temppid = pop_front_Pid_Q(&tempjob.pids);
		  if(waitpid(temppid ,&status ,WNOHANG ) == 0)
		  {
			iscompleted = false;
			push_back_Pid_Q(&tempjob.pids, temppid);
		  }
		  else
		  {
			iscompleted = true;
		  }
		}
		if(iscompleted)
		{
			print_job_bg_complete(tempjob.jobId, temppid, tempjob.cmd);
			free(tempjob.cmd);
			destroy_Pid_Q(&tempjob.pids);
		}
		else
		{
			push_back_Job_Q(&jobs, tempjob);
		}
	  }
  }
  
  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path
void run_generic(GenericCommand cmd) {
  // Execute a program with a list of arguments. The `args` array is a NULL
  // terminated (last string is always NULL) list of strings. The first element
  // in the array is the executable
  char* exec = cmd.args[0];
  char** args = cmd.args;

  // TODO: Remove warning silencers
  (void) exec; // Silence unused variable warning
  (void) args; // Silence unused variable warning
	
  // TODO: Implement run generic
  execvp(exec, args);
  //IMPLEMENT_ME();

  //perror("ERROR: Failed to execute program");
}

// Print strings
void run_echo(EchoCommand cmd) {
  // Print an array of strings. The args array is a NULL terminated (last
  // string is always NULL) list of strings.
  char** str = cmd.args;

  // TODO: Remove warning silencers
  //(void) str; // Silence unused variable warning

  // TODO: Implement echo
  //IMPLEMENT_ME();

  while (*str != NULL)
  {
    printf("%s", *str);
    str++;
  }
  printf("\n");
  
  // Flush the buffer before returning
  fflush(stdout);
}

// Sets an environment variable
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  // TODO: Remove warning silencers
  (void) env_var; // Silence unused variable warning
  (void) val;     // Silence unused variable warning

  // TODO: Implement export.
  setenv(env_var, val, 1);
  // HINT: This should be quite simple.
  //IMPLEMENT_ME();
}

// Changes the current working directory
void run_cd(CDCommand cmd) {
  // Get the directory name
  const char* dir = cmd.dir;
	char* absdir = realpath(dir, NULL);
	// Check if the directory is valid
  if (absdir == NULL) {
    perror("ERROR: Failed to resolve path");
    return;
  }
	//char* temp = getenv("PWD");
  // TODO: Change directory
	chdir(absdir);
  // TODO: Update the PWD environment variable to be the new current working
  // directory and optionally update OLD_PWD environment variable to be the old
  // working directory.
		//if(setenv("OLD_PWD", temp, 1) != 0)
		//{
			//return;
		//}
		setenv("PWD", absdir, 1);
		//{
			//return;
		//}
		free(absdir);
	//}
	//else
	//{
		//printf("Error.\n");
	//}
  //IMPLEMENT_ME();
}

// Sends a signal to all processes contained in a job
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  // TODO: Remove warning silencers
  //(void) signal; // Silence unused variable warning
  //(void) job_id; // Silence unused variable warning
	int JL = length_Job_Q(&jobs);
	for(int i=0; i<JL; i++)
	{
		job_t temp_job = pop_front_Job_Q(&jobs);
		if(temp_job.jobId == job_id)
		{
			int PL = length_Pid_Q(&temp_job.pids);
			for(int j=0; j<PL; j++)
			{
				pid_t temp_pid = pop_front_Pid_Q(&temp_job.pids);
				kill(temp_pid, signal);
				//push_back_Pid_Q(&temp_job.pids, temp_pid);
			}
		}
		else
		{
			push_back_Job_Q(&jobs, temp_job);
		}
	}
  // TODO: Kill all processes associated with a background job
  //IMPLEMENT_ME();
}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  //IMPLEMENT_ME();
  bool Free;
  char* d = get_current_directory(&Free);
  fprintf(stdout, "%s\n", d);
  // Flush the buffer before returning
  fflush(stdout);
  if(Free)
  {
	  free(d);
  }
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  //IMPLEMENT_ME();
  if(!is_empty_Job_Q(&jobs))
  {
	  for(int i=0; i<length_Job_Q(&jobs); i++)
	  {
		  job_t tempjob = pop_front_Job_Q(&jobs);
		  print_job(tempjob.jobId, peek_front_Pid_Q(&tempjob.pids), tempjob.cmd);
		  push_back_Job_Q(&jobs, tempjob);
	  }
  }
  else 
  {
	  return;
  }

  // Flush the buffer before returning
  fflush(stdout);
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for child processes.
 *
 * This version of the function is tailored to commands that should be run in
 * the child process of a fork.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case EXPORT:
  case CD:
  case KILL:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for the quash process.
 *
 * This version of the function is tailored to commands that should be run in
 * the parent process (quash).
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief Creates one new process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Processes are not the same as jobs. A single job can have multiple
 * processes running under it. This function creates a process that is part of a
 * larger job.
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
void create_process(CommandHolder holder, job_t* job) {
  // Read the flags field from the parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND; // This can only be true if r_out
                                               // is true

  // TODO: Remove warning silencers
  (void) p_in;  // Silence unused variable warning
  (void) p_out; // Silence unused variable warning
  (void) r_in;  // Silence unused variable warning
  (void) r_out; // Silence unused variable warning
  (void) r_app; // Silence unused variable warning

  // TODO: Setup pipes, redirects, and new process
  //IMPLEMENT_ME();
  pid_t pid = fork();
  if(p_out)
  {
	  //IMPLEMENT_ME();
	  pipe(pipe1);
  }
  push_back_Pid_Q(&job->pids, pid);
  //Child//
	if(pid == 0)
	{
		//IMPLEMENT_ME();
		if(p_in)
		{
			dup2(pipe1[0], STDIN_FILENO);
			//close(pipe1[1]);
		}
		if(p_out)
		{
			dup2(pipe1[1], STDOUT_FILENO);
			//close(pipe1[0]);
		}
		if(r_in)
		{
			int in_file = open(holder.redirect_in, O_RDONLY, 0);
			dup2(in_file, STDIN_FILENO);
			close(in_file);
		}
		if(r_out && r_app)
		{
			FILE* out_file = fopen(holder.redirect_out,"a");
			dup2(fileno(out_file), STDOUT_FILENO);
			fclose(out_file);
		}
		if(r_out)
		{
			FILE* out_file = fopen(holder.redirect_out, "w");
			dup2(fileno(out_file), STDOUT_FILENO);
			fclose(out_file);
		}
		//IMPLEMENT_ME();
		child_run_command(holder.cmd); // This should be done in the child branch of a fork
		exit(EXIT_SUCCESS);
	}
	else//Parent//
	{
		//IMPLEMENT_ME();
		if(p_out)
		{
			close(pipe1[1]);
		}
		push_back_Pid_Q(&job->pids, pid);
		parent_run_command(holder.cmd); // This should be done in the parent branch of
                                  // a fork
	}
  
  
}

// Run a list of commands
void run_script(CommandHolder* holders) {
	if(!isJob_run)
	{
		jobs = new_Job_Q(1);
		isJob_run = true;
	}
  if (holders == NULL)
    return;

  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }
  
  CommandType type;
  job_t Cur_job;
  Cur_job.cmd = get_command_string();
  Cur_job.pids = new_Pid_Q(1);
  
  // Run all commands in the `holder` array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i)
    create_process(holders[i], &Cur_job);

  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background Job
    // TODO: Wait for all processes under the job to complete
	while(!is_empty_Pid_Q(&Cur_job.pids))
	{
		int status;
		pid_t tmp_pid = pop_front_Pid_Q(&Cur_job.pids);
		waitpid(tmp_pid, &status, 0);
	}
	
	destroy_Pid_Q(&Cur_job.pids);
	free(Cur_job.cmd);
    //IMPLEMENT_ME();
  }
  else {
    // A background job.
    // TODO: Push the new job to the job queue
    //IMPLEMENT_ME();
	if(!is_empty_Job_Q(&jobs))
	{
		Cur_job.jobId = peek_back_Job_Q(&jobs).jobId + 1;
	}
	else
	{
		Cur_job.jobId = 1;
	}
	//IMPLEMENT_ME();
	push_back_Job_Q(&jobs, Cur_job);
    // TODO: Once jobs are implemented, uncomment and fill the following line
    print_job_bg_start(Cur_job.jobId, peek_front_Pid_Q(&Cur_job.pids), Cur_job.cmd);
  }
}
