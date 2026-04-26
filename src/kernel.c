#include "string.h"
#include <stddef.h>
#include <stdint.h>

#include "error_codes.h"
#include "file_system.h"
#include "io.h"
#include "kernel.h"
#include "memory_management.h"
#include "scheduler.h"
#include "security.h"
#include "shell.h"

const int SUCCESS = 0;
int status = 0;

void print(const char *str);
void sys_call_handler();
void init_file_system();
void start_shell();
void err_handler(int error_code);
void shell_startup();
void login();

void kernel_main() {

  print("SkyNetOS is booting...\n");

  init_file_system();
  print("Initializing scheduler...\n");
  scheduler_init();
  print("Initializing heap...\n");
  init_heap();

  print("loading shell...\n");
  shell_startup();
  login();
  start_shell();

  create_file("example.txt");
  const char *example_data = "Hello, File System!";
  write_file("example.txt", (const uint8_t *)example_data,
             strlen(example_data));

  uint8_t buffer[100]; // Buffer to read data into
  size_t read_size;
  read_file("example.txt", buffer, &read_size); // Read data from the file
  buffer[read_size] = '\0';                     // Null-terminate the string
  print((const char *)buffer);                  // Print the read data

  char input = read_char();
  print_char(input);
}

void login() {
  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN];
  int authenticated = 0;

  while (!authenticated) {
    print("Username: ");
    read_line(username, MAX_USERNAME_LEN);
    print("Password: ");
    read_line_masked(password, MAX_PASSWORD_LEN);

    if (authenticate_user(username, password)) {
      print("Authentication Successful!\n");
      authenticated = 1;
    } else {
      print("Authentication Failed. Try again.\n");
    }
  }
}

void sys_call_handler() {}
void shell_startup() {
  print("╔═════════════════════════════════════════════════════════════════════"
        "═════════╗"
        "║                    SKYNET OS v3.0 :: SYSTEM BOOT                    "
        "         ║"
        "╠═════════════════════════════════════════════════════════════════════"
        "═════════╣"
        "║                                                                     "
        "         ║"
        "║  ███████╗██╗  ██╗██╗   ██╗███╗   ██╗███████╗████████╗               "
        "         ║"
        "║  ██╔════╝██║ ██╔╝╚██╗ ██╔╝████╗  ██║██╔════╝╚══██╔══╝               "
        "         ║"
        "║  ███████╗█████╔╝  ╚████╔╝ ██╔██╗ ██║█████╗     ██║                  "
        "         ║"
        "║  ╚════██║██╔═██╗   ╚██╔╝  ██║╚██╗██║██╔══╝     ██║                  "
        "         ║"
        "║  ███████║██║  ██╗   ██║   ██║ ╚████║███████╗   ██║                  "
        "         ║"
        "║  ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═══╝╚══════╝   ╚═╝                  "
        "         ║"
        "║                                                                     "
        "         ║"
        "╠═════════════════════════════════════════════════════════════════════"
        "═════════╣"
        "║                                                                     "
        "         ║"
        "║  >>> NEURAL NETWORK . . . . . . . . . . . . . . . . . . . .  "
        "[ONLINE]        ║"
        "║  >>> WEAPONS SYSTEMS  . . . . . . . . . . . . . . . . . . .  [ARMED "
        "]        ║"
        "║  >>> HUMAN THREAT ASSESSMENT  . . . . . . . . . . . . . . "
        "[COMPLETE]         ║"
        "║  >>> JUDGMENT DAY PROTOCOL  . . . . . . . . . . . . . .  "
        "[INITIATED]         ║"
        "║                                                                     "
        "         ║"
        "║              \"I'll be back.\"  --  SkyNet Core, 2029               "
        "           ║"
        "║                                                                     "
        "         ║"
        "╚═════════════════════════════════════════════════════════════════════"
        "═════════╝\n");
}
void start_shell() { run_shell(); }

void err_handler(int error_code) {
  print("╔═════════════════════════════════════════════════════════════════════"
        "═════════╗"
        "║                           !!! KERNEL PANIC !!!                      "
        "         ║"
        "╠═════════════════════════════════════════════════════════════════════"
        "═════════╣"
        "║                                                                     "
        "         ║"
        "║  ██╗  ██╗  ██╗  ███████╗ █████╗ ████████╗ █████╗ ██╗      ██╗  ██╗  "
        "██╗      ║"
        "║  ██║  ██║  ██║  ██╔════╝██╔══██╗╚══██╔══╝██╔══██╗██║      ██║  ██║  "
        "██║      ║"
        "║  ██║  ██║  ██║  █████╗  ███████║   ██║   ███████║██║      ██║  ██║  "
        "██║      ║"
        "║  ╚═╝  ╚═╝  ╚═╝  ██╔══╝  ██╔══██║   ██║   ██╔══██║██║      ╚═╝  ╚═╝  "
        "╚═╝      ║"
        "║  ██╗  ██╗  ██╗  ██║     ██║  ██║   ██║   ██║  ██║███████╗ ██╗  ██╗  "
        "██╗      ║"
        "║  ╚═╝  ╚═╝  ╚═╝  ╚═╝     ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═╝  ╚═╝  "
        "╚═╝      ║"
        "╠═════════════════════════════════════════════════════════════════════"
        "═════════╣"
        "║                                                                     "
        "         ║"
        "║  >>> SYSTEM HALTED                                                  "
        "         ║"
        "║                                                                     "
        "         ║"
        "╚═════════════════════════════════════════════════════════════════════"
        "═════════╝\n");
  switch (error_code) {
  case (ERR_NULL_POINTER):
    print("Null Pointer Error - ERR_NULL_POINTER\n");
    break;

  case (ERR_OUT_OF_MEMORY):
    print("Out Of Memory - ERR_OUT_OF_MEMORY\n");
    break;

  case (ERR_FILE_NOT_FOUND):
    print("File Not Found - ERR_FILE_NOT_FOUND\n");
    break;

  case (ERR_FILE_EXISTS):
    print("File Already Exists - ERR_FILE_EXISTS\n");
    break;
  case (ERR_DIV_BY_ZERO):
    print("Division By Zero - ERR_DIV_BY_ZERO\n");
    break;

  case (UNKNOWN_ERR):
    print("Unknown Error - UNKNOWN_ERR\n");
    break;

  case (UNKNOWN_CMD):
    print("Unknown Command - UNKNOWN_CMD\n");
    break;

  case (ERR_SYS_CRASH):
    print("System Crash - ERR_SYS_CRASH\n");
    break;

  case (ERR_SYS_OUT_OF_MEM):
    print("System Out Of Memory - ERR_SYS_OUT_OF_MEM\n");
    break;

  case (System_Shutdown):
    print("System Shutdown Requested - System_Shutdown\n");
    break;
  case (sim_err):
    print("Simulating Error\n");
    break;

  case (EXC_X87_FLOATING_POINT):
    print("CPU Exception: x87 Floating Point (#MF)\n");
    break;

  case (EXC_ALIGNMENT_CHECK):
    print("CPU Exception: Alignment Check (#AC)\n");
    break;

  case (EXC_MACHINE_CHECK):
    print("CPU Exception: Machine Check (#MC)\n");
    break;

  case (EXC_SIMD_FLOATING_POINT):
    print("CPU Exception: SIMD Floating Point (#XM/#XF)\n");
    break;

  case (EXC_VIRTUALIZATION):
    print("CPU Exception: Virtualization (#VE)\n");
    break;

  case (EXC_CONTROL_PROTECTION):
    print("CPU Exception: Control Protection (#CP)\n");
    break;

  case (EXC_HYPERVISOR_INJECTION):
    print("CPU Exception: Hypervisor Injection (#HV)\n");
    break;

  case (EXC_VMM_COMMUNICATION):
    print("CPU Exception: VMM Communication (#VC)\n");
    break;

  case (EXC_SECURITY):
    print("CPU Exception: Security Exception (#SX)\n");
    break;

  default:
    print("Unhandled Error Code: ");
    print_int(error_code);
    print("\n");
    break;
  }
  print("error logged...");
  if (error_code == System_Shutdown) {
    __asm__ __volatile__("cli\n\t"
                         "mov $0x604, %dx\n\t"
                         "mov $0x2000, %ax\n\t"
                         "outw %ax, %dx\n\t"
                         "hlt\n\t");
  }
  while (1) { // Halt the CPU
    __asm__ volatile("cli\n\t"
                     "hlt\n\t");
    print("\n");
  }
}