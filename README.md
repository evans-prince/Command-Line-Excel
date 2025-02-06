This is our Spreadsheet program in C.

• main.c (input loop) → input_handler.c (command router) → specialized modules (formulas, scrolling, etc.)

# *THIS SECTION DISCUSSES ABOUT THE 'spreadsheet.h' FILE*
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


# *THIS SECTION DISCUSSES INPUT_HANDLER FILE*

* As input is evaluated one input at a time we didn't do memory analysis in that deep as while defining spreadsheet.h  that object would take memory for each cell and memory management is crucial there , however here few extra bytes won't affect for all practical purposes.

* Hence we have decided for the sake of modularity of code instead of direct parsing the input and calling functions when necessarily we first initialised the object input containing several fields includeing input_type ; default set to NULL and NOT_DECIDED respectively then parse it proceed. This decision convinced us that if needed we can add further functionality to it without much effort (compared to if we have parsed directly without any input struct), debugging is also esay as well as testing and understanding of code. Also, by doing this we have realised the importance of OOPS.

* For the ease of readability we have used enumeration when defining input_type.

 **'is_cell_value_assignment'** function- This checks if the input is an expression assignment in numerical values for a cell.
Here, we splitted the input string into two parts using the 1st '=' in the input string as the delimiter. Then we extracted the two parts of the string and check whether those are valid. The string before '=' should be a cell name and the string after '=' should be a numerical expression assignment. We built separate function for checking these conditions.

Now coming to *is_cell_name* function, it checks whether there is a continuous stream of alphabets and then a continuous stream of digits and that alphabets only occur before digits.
The primary condition should be that the cell name string passed to this function should have length greater than 2 and less than 6. We also added a count for alphabets and digits to ensure that they are atleast 1 and atmost 3 each.

Now coming to the *is_expression* function, 


** --------------------MORE TO BE ADDED : WIP -------------------------------- 
