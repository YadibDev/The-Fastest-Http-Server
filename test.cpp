#include <iostream>
#include <unistd.h>   // For pipe, fork, execve
#include <sys/wait.h> // For wait
#include <vector>
#include <fcntl.h>

struct stEventProcess
{
    enum eEventProcess {RUNINNG, THE_END, END_WITH_PARSE, END_WITH_TIMOUT = 504, END_UNKNOW = 500};
};

stEventProcess::eEventProcess checkProcessStatus(int pid)
{
    int status;
	int exit_code = waitpid(pid, &status, WNOHANG);

	if (exit_code != 0)
	{
        if (WIFEXITED(status)) // if exited
        {
            if (WEXITSTATUS(status) != 0) // check status
    			return stEventProcess::END_UNKNOW;
            else
                return stEventProcess::THE_END;
        }
        else if (WIFSIGNALED(status) || exit_code == -1)
    		return stEventProcess::END_UNKNOW;
		return stEventProcess::THE_END;
	}
	return stEventProcess::RUNINNG;
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // --- CHILD PROCESS ---
        // Close the read end of the pipe (child only writes)
        close(pipefd[0]);

        // Redirect stdout (1) to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Arguments for cat: [program_name, target_file, nullptr]
        char* binaryPath = (char*)"/bin/echo";
        char* args[] = {binaryPath, "00000990", nullptr};
        
        // Environment variables (empty for this example)
        char* env[] = {nullptr};

        // Replace current process image with cat
        execve(binaryPath, args, env);

        // execve only returns if there is an error
        perror("execve");
        return 1;

    } else { // --- PARENT PROCESS ---
        // Close the write end of the pipe (parent only reads)
        close(pipefd[1]);

        char buffer[128];
        ssize_t bytesRead;

        std::cout << "--- Parent reading from pipe ---" << std::endl;

        // Read from the pipe until it's empty/closed
        std::cout << fcntl(pipefd[0], F_GETFD) << std::endl;;
        O_RDONLY
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            std::cout << buffer;
        }
        int i = 0;
        while (1)
        {
            sleep(1);
            if (checkProcessStatus(pid) == stEventProcess::RUNINNG)
            {
                std::cout << "test\n" << std::endl;;
                kill(pid, SIGKILL);
            }
            std::cout << checkProcessStatus(pid) << std::endl;;
            i++;
        }

        close(pipefd[0]);
    }

    return 0;
}