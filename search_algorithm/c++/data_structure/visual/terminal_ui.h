#include <ncurses.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Uses ncurses to create a simple terminal UI
class TerminalUI {
   public:
    using WindowPtr = std::unique_ptr<WINDOW, decltype(&delwin)>;

    // Columns: how many columns to split the screen into
    TerminalUI(uint8_t columns = 2) : columns_(columns) {
        if (columns_ != 2)
            // WIP
            throw std::invalid_argument("Only 2 columns supported for now");

        initscr();             // Initialize the window
        cbreak();              // Disable line buffering
        noecho();              // Don't echo user input
        keypad(stdscr, TRUE);  // Enable function keys and arrow keys
        curs_set(0);           // Hide the cursor

        getmaxyx(stdscr, max_height_, max_width_);

        column_width_ = max_width_ / columns_;
        column_height_ = max_height_ - 1;  // Leave space for status bar

        for (uint8_t i = 0; i < columns_; ++i) {
            windows_.emplace_back(WindowPtr(
                newwin(column_height_, column_width_, 0, i * column_width_),
                &delwin));
        }
    }

    ~TerminalUI() {
        endwin();  // Restore normal terminal behavior
    }

    void Clear() { clear(); }

    void RefreshAll() {
        // Refresh stdscr
        refresh();

        // Refresh all windows
        for (const auto& window : windows_) wrefresh(window.get());
    }

    void PrintToWindow(int window_index, int y, int x, const std::string& str) {
        WINDOW* window_ptr = windows_[window_index].get();
        mvwprintw(window_ptr, y, x, "%s", str.c_str());
    }

    void PrintToStatusBar(const std::string& str) {
        mvprintw(max_height_ - 1, 0, "%s", str.c_str());
        clrtoeol();  // Clear only the status bar to remove any leftover text
        refresh();   // Refresh only stdscr to update the status bar
    }

    int GetInput() {
        return getch();  // Use ncurses getch() instead of std::cin
    }

    void WaitForKey(int key) {
        int ch;
        do {
            ch = getch();
        } while (ch != '\n' && ch != '\r' && ch != key);
    }

   private:
    uint8_t columns_;
    uint16_t max_width_, max_height_;        // Max dimensions of the terminal
    uint16_t column_width_, column_height_;  // Dimensions of each column
    std::vector<WindowPtr> windows_;
};