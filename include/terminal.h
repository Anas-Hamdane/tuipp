#pragma once
#include <iostream>

namespace tuipp::terminal
{
inline void clear() { std::cout << "\x1b[2J\x1b[H"; }

// follow it by std::cout.flush()
inline void enterAlternate() { std::cout << "\x1b[?1049h"; }

inline void leaveAlternate() { std::cout << "\x1b[?1049l"; }

inline void hideCursor() { std::cout << "\x1b[?25l"; }

inline void showCursor() { std::cout << "\x1b[?25h"; }

inline void moveCursor(int x, int y)
{
  std::cout << "\x1b[" << y << ";" << x << "H";
}

inline void flush() { std::cout.flush(); }

#if defined(__unix__) || defined(__APPLE__)
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static struct termios original;
inline void enableRawMode()
{
  tcgetattr(STDIN_FILENO, &original);
  struct termios raw = original;

  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);

  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
inline void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original); }

inline std::pair<int, int> size()
{
  struct winsize ws{};
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

  return {static_cast<int>(ws.ws_col), static_cast<int>(ws.ws_row)};
}
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>

static DWORD original;
inline void enableRawMode()
{
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(hIn, &original);

  DWORD raw = original;
  raw &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);

  SetConsoleMode(hIn, raw);
}
inline void disableRawMode()
{
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  SetConsoleMode(hIn, original);
}

inline std::pair<int, int> size()
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleScreenBufferInfo(hOut, &csbi);

  int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  return {width, height};
}
#endif

} // namespace tuipp::terminal
