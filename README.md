This is our Spreadsheet program in C.

• main.c (input loop) → input_handler.c (command router) → specialized modules (formulas, scrolling, etc.)

# spreadsheet.h (More details to be added)
• This file defines the core structures and functions for the Command-Line-Excel program. 

• The 'cell' structure includes a value and dependency boolean which tracks whether the current cell is explicitly containing a value or is dependent on some other cell.

• The 'sheet' structure includes the 2D grid, the number of rows, the number of columns, and the sheet's bounds. The bounds specify the indices of the first row and first column that are visible in the terminal.

• The 'val' and 'dependency' are kept as normal variables due to simplicity and also to reduce the memory overhead of dynamic allocation.
The int val takes 4 bytes and bool dependency takes 1 byte but due to structure padding it also takes 4 bytes so in total 8 bytes.
And int *val will take 8 bytes so there the total would be 16 bytes.
Considering the extreme case when we have 999 rows and 18,278 columns, the memory required to store the spreadsheet in normal variables case would be around 140 MB and in dynamic case it would be around 280 MB.
Also, dereferencing a pointer takes time which can be minimized if we use normal variables instead of pointers.

Thus, using normal variables significantly reduces the memory and time constraints of our program.

• In spreadsheet_bounds- As there is only one instance of spreadsheet_bounds, normal variables are simpler and sufficient. Normal variables also avoid the overhead of dynamic allocation and pointer dereferencing.
