#include <Kernel/API/Syscall.h>
#include <Kernel/Process/Console/Console.h>
#include <Utils/Memory.h>
#include <Utils/Types.h>

int strcmp(const char* str1, const char* str2) {
  for(; *str1 && *str2; ++str1, ++str2) {
    if(*str1 != *str2) {
      return *str1 - *str2;
    }
  }

  return *str1 - *str2;
}

void print_help() {
  const char* help_message = "Available commands:\n"
                             "1. dmesg - Print the kernel log\n"
                             "2. stats - Print the system stats\n"
                             "3. excel - Print a message to Excel@FIT\n"
                             "4. ls - List files in the current directory\n"
                             "5. mem - Print memory statistics\n"
                             "6. help - Show this help message\n";

  syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>(help_message));
}

extern "C" int console_main() {
  auto fd = static_cast<int>(syscall(Kernel::Syscalls::Sys$open, reinterpret_cast<uintptr_t>("/dev/tty0"), 0));
  if(fd < 0) {
    syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Couldn't open the tty."));
    while(true) {}
  }

  static constexpr char matte_os_logo[] =
          "MMMMMMMM               MMMMMMMM                          tttt               tttt                                   OOOOOOOOO        SSSSSSSSSSSSSSS \nM:::::::M             M:::::::M                       ttt:::t            ttt:::t                                 OO:::::::::OO    SS:::::::::::::::S\nM::::::::M           M::::::::M                       t:::::t            t:::::t                               OO:::::::::::::OO S:::::SSSSSS::::::S\nM:::::::::M         M:::::::::M                       t:::::t            t:::::t                              O:::::::OOO:::::::OS:::::S     SSSSSSS\nM::::::::::M       M::::::::::M  aaaaaaaaaaaaa  ttttttt:::::tttttttttttttt:::::ttttttt        eeeeeeeeeeee    O::::::O   O::::::OS:::::S            \nM:::::::::::M     M:::::::::::M  a::::::::::::a t:::::::::::::::::tt:::::::::::::::::t      ee::::::::::::ee  O:::::O     O:::::OS:::::S            \nM:::::::M::::M   M::::M:::::::M  aaaaaaaaa:::::at:::::::::::::::::tt:::::::::::::::::t     e::::::eeeee:::::eeO:::::O     O:::::O S::::SSSS         \nM::::::M M::::M M::::M M::::::M           a::::atttttt:::::::tttttttttttt:::::::tttttt    e::::::e     e:::::eO:::::O     O:::::O  SS::::::SSSSS    \nM::::::M  M::::M::::M  M::::::M    aaaaaaa:::::a      t:::::t            t:::::t          e:::::::eeeee::::::eO:::::O     O:::::O    SSS::::::::SS  \nM::::::M   M:::::::M   M::::::M  aa::::::::::::a      t:::::t            t:::::t          e:::::::::::::::::e O:::::O     O:::::O       SSSSSS::::S \nM::::::M    M:::::M    M::::::M a::::aaaa::::::a      t:::::t            t:::::t          e::::::eeeeeeeeeee  O:::::O     O:::::O            S:::::S\nM::::::M     MMMMM     M::::::Ma::::a    a:::::a      t:::::t    tttttt  t:::::t    tttttte:::::::e           O::::::O   O::::::O            S:::::S\nM::::::M               M::::::Ma::::a    a:::::a      t::::::tttt:::::t  t::::::tttt:::::te::::::::e          O:::::::OOO:::::::OSSSSSSS     S:::::S\nM::::::M               M::::::Ma:::::aaaa::::::a      tt::::::::::::::t  tt::::::::::::::t e::::::::eeeeeeee   OO:::::::::::::OO S::::::SSSSSS:::::S\nM::::::M               M::::::M a::::::::::aa:::a       tt:::::::::::tt    tt:::::::::::tt  ee:::::::::::::e     OO:::::::::OO   S:::::::::::::::SS \nMMMMMMMM               MMMMMMMM  aaaaaaaaaa  aaaa         ttttttttttt        ttttttttttt      eeeeeeeeeeeeee       OOOOOOOOO      SSSSSSSSSSSSSSS\n";


  syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>(matte_os_logo));

  char buffer[512] = {};
  while(true) {
    if(syscall(Kernel::Syscalls::Sys$read, fd, reinterpret_cast<uintptr_t>(buffer), sizeof(buffer)) > 0) {
      if(!strcmp(buffer, "dmesg")) {
        syscall(Kernel::Syscalls::Sys$dmesg);
      } else if(!strcmp(buffer, "stats")) {
        syscall(Kernel::Syscalls::Sys$stats);
      } else if(!strcmp(buffer, "excel")) {
        syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Hi, Excel@FIT!"));
      } else if(!strcmp(buffer, "ls")) {
        syscall(Kernel::Syscalls::Sys$ls);
      } else if(!strcmp(buffer, "mem")) {
        syscall(Kernel::Syscalls::Sys$mem);
      } else if(!strcmp(buffer, "help")) {
        print_help();
      } else {
        syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Unknown command. Try help to see the list of commands."));
      }
    }
    // Clear the buffer for the next command just in case
    Utils::memset(buffer, 0, 512);
  }

  Kernel::syscall(Kernel::Syscalls::Sys$exit, 0);
}