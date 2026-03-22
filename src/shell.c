#include "string.h"
#include "io.h" 
#include "shell.h"  
#include "kernel.h"
#include "error_codes.h"
#include "scheduler.h"
#include "gui.h"
#include "vga.h"
#include "file_system.h"
#include "memory_management.h"
#include "disk_scheduler.h"

void handle_command(const char* command);
void print_help();

void ps_command() {
    tcb_t info;
    print("PID\tPRIO\tSTATE\tTHRDS\tMSGS\n");
    for (int i = 0; i < MAX_TASKS; i++) {
        if (get_task_info(i, &info)) {
            print_int(info.pid); print("\t");
            print_int(info.priority_level); print("\t");
            switch (info.state) {
                case TASK_RUNNING: print("RUN\t"); break;
                case TASK_READY:   print("RDY\t"); break;
                case TASK_BLOCKED: print("BLK\t"); break;
                default:           print("UNK\t"); break;
            }
            print_int(info.thread_count); print("\t");
            print_int(info.msg_count); print("\n");
        }
    }
}

void run_shell() {
    char command[100];
    while (1) {  
        print("SkyNet > "); 
        read_line(command, sizeof(command));
        if (strlen(command) == 0) continue;
        handle_command(command);
    }
}

// External file system helpers
extern directory_t* get_root_directory(void);

void handle_command(const char* command) {
    bool background = false;
    int len = strlen(command);
    char cmd_buffer[100];
    strcpy(cmd_buffer, command);

    if (len > 2 && cmd_buffer[len-1] == '&') {
        background = true;
        cmd_buffer[len-1] = '\0';
        if (cmd_buffer[len-2] == ' ') cmd_buffer[len-2] = '\0';
    }

    if (strcmp(cmd_buffer, "help") == 0) {
        print_help();
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        clear_screen();
    } else if (strcmp(cmd_buffer, "whoami") == 0) {
        print("admin\n");
    } else if (strcmp(cmd_buffer, "ls") == 0) {
        directory_t* root = get_root_directory();
        if (root->file_count == 0) {
            print("(empty)\n");
        } else {
            for (size_t i = 0; i < root->file_count; i++) {
                if (root->files[i]->is_directory) {
                    print("[DIR]  ");
                } else {
                    print("[FILE] ");
                }
                print(root->files[i]->name);
                print("  (");
                print_int((int)root->files[i]->size);
                print(" bytes)\n");
            }
        }
    } else if (strncmp(cmd_buffer, "cat ", 4) == 0) {
        const char* filename = cmd_buffer + 4;
        uint8_t buffer[512];
        size_t read_size;
        int result = read_file(filename, buffer, &read_size);
        if (result == 0 && read_size > 0) {
            buffer[read_size] = '\0';
            print((const char*)buffer);
            print("\n");
        } else {
            print("File not found: ");
            print(filename);
            print("\n");
        }
    } else if (strncmp(cmd_buffer, "touch ", 6) == 0) {
        const char* filename = cmd_buffer + 6;
        create_file(filename);
        print("Created file: ");
        print(filename);
        print("\n");
    } else if (strncmp(cmd_buffer, "mkdir ", 6) == 0) {
        const char* dirname = cmd_buffer + 6;
        create_directory(dirname);
        print("Created directory: ");
        print(dirname);
        print("\n");
    } else if (strncmp(cmd_buffer, "rm ", 3) == 0) {
        const char* filename = cmd_buffer + 3;
        delete_file(filename);
        print("Deleted: ");
        print(filename);
        print("\n");
    } else if (strncmp(cmd_buffer, "edit ", 5) == 0) {
        const char* filename = cmd_buffer + 5;
        uint8_t file_buffer[4096];
        size_t current_size = 0;
        
        int res = read_file(filename, file_buffer, &current_size);
        if (res != 0) {
            create_file(filename);
            current_size = 0;
        } else if (current_size > 0) {
            print("--- Current File Contents ---\n");
            file_buffer[current_size] = '\0';
            print((char*)file_buffer);
            if (file_buffer[current_size - 1] != '\n') {
                print("\n");
            }
        }
        
        print("--- Edit Mode ---\n");
        print("Type your text. Type ':wq' on a new line to save and exit.\n");
        char line_buf[256];
        while (1) {
            read_line(line_buf, sizeof(line_buf));
            if (strcmp(line_buf, ":wq") == 0) {
                break;
            }
            size_t line_len = strlen(line_buf);
            if (current_size + line_len + 1 < sizeof(file_buffer)) {
                memcpy(file_buffer + current_size, line_buf, line_len);
                current_size += line_len;
                file_buffer[current_size++] = '\n';
            } else {
                print("Buffer full! Cannot add more text.\n");
            }
        }
        write_file(filename, file_buffer, current_size);
        print("File saved.\n");
    } else if (strcmp(cmd_buffer, "ps") == 0) {
        ps_command();
    } else if (strncmp(cmd_buffer, "kill ", 5) == 0) {
        int pid = cmd_buffer[5] - '0';
        task_terminate(pid);
    } else if (strncmp(cmd_buffer, "thread-add ", 11) == 0) {
        int pid = cmd_buffer[11] - '0';
        if (!thread_create(pid)) print("Failed to add thread.\n");
    } else if (strncmp(cmd_buffer, "ipc-send ", 9) == 0) {
        int pid = cmd_buffer[9] - '0';
        if (cmd_buffer[10] == ' ') {
            if (!ipc_send(pid, cmd_buffer + 11)) print("IPC send failed.\n");
        } else {
            print("Usage: ipc-send <pid> <msg>\n");
        }
    } else if (strncmp(cmd_buffer, "ipc-recv ", 9) == 0) {
        int pid = cmd_buffer[9] - '0';
        char msg_buf[64];
        if (ipc_receive(pid, msg_buf)) {
            print("Received: "); print(msg_buf); print("\n");
        } else {
            print("No messages or IPC failed.\n");
        }
    } else if (strcmp(cmd_buffer, "disk-test") == 0) {
        init_disk_scheduler();
        submit_disk_request(98);
        submit_disk_request(183);
        submit_disk_request(37);
        submit_disk_request(122);
        submit_disk_request(14);
        submit_disk_request(124);
        submit_disk_request(65);
        submit_disk_request(67);
        process_disk_requests(53); // Starting head position
    } else if (strcmp(cmd_buffer, "exit") == 0) {
        print("Exiting shell...\n");
        err_handler(System_Shutdown);
    } else if (strcmp(cmd_buffer, "sim-load") == 0) {
        if (background) {
            int res[] = {2, 2, 2};
            int pid = task_create(res);
            print("Started simulated load task with PID: ");
            print_int(pid); print("\n");
        } else {
            print("Running simulated load synchronously...\n");
            for(volatile int i=0; i<1000000; i++);
            print("Done.\n");
        }
    } else if (strcmp(cmd_buffer, "gui") == 0) {
        print("Entering GUI mode...\n");
        gui_init();
        gui_create_window("System Monitor", 50, 50, 400, 300, 15);
        gui_create_window("About SkyNet", 500, 100, 250, 150, 14);
        gui_render();
        read_char();
        vga_set_text_mode();
        clear_screen();
        print("Returned to text mode.\n");
    } else {
        print("Unknown command: "); print(cmd_buffer); print("\n");
    }
}

void print_help() {
    print("Available commands:\n");
    print("  help       - Display this help message\n");
    print("  clear      - Clear the screen\n");
    print("  whoami     - Display current user\n");
    print("  ls         - List files and directories\n");
    print("  cat <file> - Display file contents\n");
    print("  touch <f>  - Create an empty file\n");
    print("  mkdir <d>  - Create a directory\n");
    print("  rm <file>  - Delete a file\n");
    print("  edit <file>- Edit or create a file\n");
    print("  ps         - List active processes\n");
    print("  kill <pid> - Terminate a process\n");
    print("  sim_load   - Run simulated workload (& for bg)\n");
    print("  gui        - Start GUI mode\n");
    print("  exit       - Shutdown the system\n");
}
