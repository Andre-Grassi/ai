/**
 * @file terminal_ui.h
 * @brief Terminal User Interface using ncurses library
 * @author Andre Grassi de Jesus
 * @date 2025
 */

#ifndef SEARCH_ALG_DATA_STRUCTURE_VISUAL_TERMINAL_UI_H_
#define SEARCH_ALG_DATA_STRUCTURE_VISUAL_TERMINAL_UI_H_

#include <ncurses.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @class TerminalUI
 * @brief A terminal-based user interface using ncurses for multi-column display
 *
 * This class provides a wrapper around ncurses functionality to create
 * a simple terminal UI with multiple columns and a status bar.
 * Currently supports 2-column layout with plans for expansion.
 */
class TerminalUI {
   public:
    /**
     * @brief Type alias for WINDOW pointer with custom deleter, because
     * the WINDOW struct does not have a delete method (C library struct)
     */
    using WindowPtr = std::unique_ptr<WINDOW, decltype(&delwin)>;

    /**
     * @brief Constructor that initializes the ncurses terminal UI
     * @param columns Number of columns to split the screen into (default: 2)
     * @throws std::invalid_argument if columns is not 2 (currently only 2
     * columns supported)
     */
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

    /**
     * @brief Destructor that properly cleans up ncurses resources
     */
    ~TerminalUI() {
        endwin();  // Restore normal terminal behavior
    }

    /**
     * @brief Clears the main screen
     */
    void Clear() { clear(); }

    /**
     * @brief Refreshes all windows and the main screen
     *
     * This method refreshes the main screen (stdscr) and all created windows
     * to update the display with any changes made since the last refresh.
     */
    void RefreshAll() {
        // Refresh stdscr
        refresh();

        // Refresh all windows
        for (const auto& window : windows_) wrefresh(window.get());
    }

    /**
     * @brief Prints text to a specific window at given coordinates
     * @param window_index Index of the window to print to (0-based)
     * @param y Y coordinate (row) within the window
     * @param x X coordinate (column) within the window
     * @param str String to print
     */
    void PrintToWindow(int window_index, int y, int x, const std::string& str) {
        WINDOW* window_ptr = windows_[window_index].get();
        mvwprintw(window_ptr, y, x, "%s", str.c_str());
    }

    /**
     * @brief Prints text to the status bar at the bottom of the screen
     * @param str String to display in the status bar
     *
     * This method prints text to the bottom line of the terminal (status bar)
     * and automatically clears any remaining text on that line.
     */
    void PrintToStatusBar(const std::string& str) {
        mvprintw(max_height_ - 1, 0, "%s", str.c_str());
        clrtoeol();  // Clear only the status bar to remove any leftover text
        refresh();   // Refresh only stdscr to update the status bar
    }

    /**
     * @brief Gets input from the user (single character)
     * @return Integer representing the key pressed
     *
     * Uses ncurses getch() to capture user input without echoing
     * to the screen.
     */
    int GetInput() {
        return getch();  // Use ncurses getch() instead of std::cin
    }

    /**
     * @brief Waits for a specific key to be pressed
     * @param key The key code to wait for
     *
     * This method blocks execution until the specified key is pressed. Other
     * key presses are ignored.
     */
    void WaitForKey(int key) {
        int ch;
        do {
            ch = getch();
        } while (ch != key);
    }

   private:
    uint8_t columns_;                 /**< Number of columns in the UI layout */
    uint16_t max_width_, max_height_; /**< Maximum dimensions of the terminal */
    uint16_t column_width_,
        column_height_; /**< Dimensions of each individual column */
    std::vector<WindowPtr> windows_; /**< Vector of ncurses window pointers */
};

#endif  // SEARCH_ALG_DATA_STRUCTURE_VISUAL_TERMINAL_UI_H_