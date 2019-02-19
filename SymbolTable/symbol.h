#ifndef __SYMBOL_H__
#define __SYMBOL_H__

/*
 * "Copyright (c) 2014 by Fritz Sieker."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written
 * agreement is hereby granted, provided that the above copyright notice
 * and the following two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE AUTHOR
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND THE AUTHOR NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 */

/** @file symbol.h
 *  @brief Defines the interface to symbol.c functions (do not modify)
 *  @details This file defines the interface to a C file symbol.c that
 *  you will complete.
 *  <p>
 *  In this implementation, you will learn about dynamic memory management using
 *  malloc/calloc/free. You will also learn about function pointers (callback
 *  functions).
 */

/** This defines an opaque type. The actual contents of the structure are hidden
 *  in the implementation and only a pointer to this structure is used
 *  externally to this file. A pointer to an opaque structure is sometimes
 *  referred to as a handle.
 */

typedef struct sym_table sym_table_t;

/** The symbol_find methods return a pointer to this data structure. It
represents a (name, address) pair in the symbol table.
 */

typedef struct symbol {
    char* name; /**< the name of the symbol */
    int   addr; /**< symbol's address in the LC3 memory */
} symbol_t;

/** Defines the signature of a callback function (also known as a function
 *  pointer). This is how languages such as Java and C++ do <b>dynamic</b>
 *  binding (i.e. figure out which function to call). Recall that in Java, the
 *  code <code>obj.equals(object)</code> will call one of possibly many
 *  different methods depending on the actual type of <code>obj</code>. This
 *  is because the method <b>.equals()</b> may be <b>overridden</b>.
 *  <p>
 *  In the LC3, dynamic binding is based on the <b>JSRR</b> opcode. With this
 *  opcode, the address of the routine to call is stored in a register and can
 *  be changed at runtime. Compare this to a <b>JSR nameOfRoutine</b> opcode 
 *  which specifies what routine to call from the label that follows it. Thus, 
 *  the address is fixed at <b>assembly</b> time. 
 *  <p>
 *  This is used in the <code>symbol_iterate</code> function.
 *  @param sym - pointer to a symbol
 *  @param data - a pointer to some data that the function might need.
 */
typedef void (*iterate_fnc_t)(symbol_t* sym, void* data);

/** Create a new symbol table and return a pointer to it. This function is a
 *  constructor for a symbol table and is automatically called when the program
 *  starts. In this function, you must dynamically (i.e., using
 *  <code>malloc()/calloc()</code>) allocate space for the following:
 * 
 *  <ol>
 *  <li><b>One <code>sym_table_t</code> structure:</b> it is important to note
 *      that this structure does not contain the actual data of the symbol
 *      table. It's a bookkeeping structure that contains the capacity of the
 *      hash table, a pointer to the actual hash table, and a pointer to the
 *      address table (refer to our drawing of the data structures). Hence,
 *      don't try to allocate space for more than one <code>sym_table_t</code>
 *      structure.</li>
 *  <li><b>The hash table:</b> this is an array of <code>node_t*</code>. The
 *      capacity of the hash table is given by the <code>table_size</code>
 *      argument which comes from the command-line argument that you supply to
 *      <code>./testSymbol</code>. You can think of each element of this array
 *      as being a pointer to the head of a linked list. Hence, you must ensure
 *      that all the pointers in the hash table are initialized to 0 (or NULL)
 *      since the table is initially empty. The <code>hash_table</code> member
 *      of the <code>sym_table_t</code> structure allocated previously must be
 *      set to point to the start of the hash table.</li>
 *  <li><b>The address table:</b> this is an array of <code>char*</code>. The
 *      number of elements is fixed. It corresponds to the number of possible
 *      addresses in the LC-3. Since an LC-3 address can only be 16 bits, the
 *      number of possible addresses is 2<sup>16</sup>. For your convenience, we
 *      have defined a constant in <code>symbol.c</code> with this value
 *      (<code>LC3_MEMORY_SIZE</code>). Each element in this array is a pointer
 *      to the start of a string. Hence, you must ensure that all the pointers
 *      in the table are initialized to 0 (or NULL) since the table is initially
 *      empty. The <code>addr_table</code> member of the
 *      <code>sym_table_t</code> structure allocated previously must be set to
 *      point to the start of the hash table.</li>
 *  </ol>
 * 
 *  Make sure you store <code>table_size</code> in the <code>size</code> member
 *  of the <code>sym_table_t</code> structure allocated previously.
 * 
 *  @param table_size - The size of the hash table.
 *  @return A pointer to the <code>sym_table_t</code> structure you
 *  allocated. This pointer will be passed to you in the other functions so that
 *  you can access the hash table and the address table.
 */ 
sym_table_t* symbol_init (int table_size);

/** Add a symbol to the symbol table. This function assumes that the name you
 *  are trying to add to the symbol table is not already associated with  an
 *  existing symbol (you do not have to check for name duplicates in this
 *  function). You must initialize the fields of the structure appropriately and
 *  store it at the correct index in the hash table. The entry at a given index
 *  of the hash table is a pointer to the head of a linked list of nodes. The
 *  symbol just created <span style="color: red; font-weight: bold;">must</span>
 *  become the new <b>head</b> of the list. If you do not follow this
 *  requirement, you may not get full credit. You also need to make the
 *  corresponding entry in the address table point to the name of the symbol
 *  that was just added (each index in the <code>addr_table</code> array
 *  represents an address). If an entry in the <code>addr_table</code> array is
 *  already pointing to a name (this happens if two names are associated with
 *  the same address), then you must not modify the existing entry
 *  <code>addr_table</code> array (but you should still add the symbol to the
 *  hash table).
 * 
 *  The general algorithm is as follows:
 * 
 *  <ol>
 *  <li>Calculate the hash of the symbol using the provided
 *      <code>symbol_hash()</code> function (defined in
 *      <code>symbol.c</code>).</li>
 *  <li>Based on the hash, calculate the index where the symbol should go in the
 *      hash table. Why don't we just use the hash as the index? Because the
 *      hash can be a very big number. It could exceed the bounds of our array.
 *      We have to "shrink" it down so that we get an index within bounds. To
 *      do so, we compute
 *      <code>index = hash-value % capacity-of-hash-table</code>.</li>
 *  <li>Create a new node (<code>node_t</code>) and initialize its members
 *      appropriately. All members of the node should be initialized. The
 *      <code>next</code> member is used to link to the next node in the linked
 *      list (this should be NULL for the last node in the list). The
 *      <code>hash</code> member should contain the hash (not the index)
 *      calculated previously. This member will be useful in the
 *      <code>symbol_search()</code> function. The <code>symbol</code> member
 *      is a structure that contains the (name, addr) pair. The name must be
 *      stored as-is, i.e., don't change uppercase or lowercase letters. See
 *      the important note below about the <code>name</code> parameter.</li>
 *  <li>Add the node to the hash table. You must be able to handle collisions.
 *      That is, it is still possible for two symbols with different names to
 *      share the same index in the hash table.</li>
 *  <li>Set the corresponding entry in the address table to point to the name
 *      of the symbol that you just added (if applicable).</li>
 *  </ol>
 * 
 *  You may test your implementation using the <code>addu</code> command, but
 *  you will not be able to see if you inserted a node until you implement the
 *  <code>symbol_find_by_addr()</code> and <code>symbol_iterate()</code>
 *  functions.
 *
 *  A hash table is designed to spread out the entries, often sacrificing space
 *  for speed. To thoroughly test your function, you will need to force multiple
 *  names to occupy the same index in the hash table (this will be tested in
 *  part A, it is not the same as duplicate symbol detection). Think about the
 *  pigeon hole principle from CS161. Your code will be tested with a variety of
 *  different values.
 * 
 *  <span style="color: red; font-weight: bold;">Important:</span>
 * 
 *  <ol>
 *  <li>Notice that in this function, you are passed <code>const char
 *      *name</code>. This is the name argument that the user enters for the
 *      <code>addu</code> command. More specifically, it is a pointer to that
 *      string in memory. You cannot assume that the string is going to remain
 *      unchanged after your function returns. The caller may choose to do
 *      anything with the memory where this pointer is pointing to. Hence, you 
 *      must create a copy of the name argument. That way, you can guarantee
 *      that the copy will remain unchanged for the duration of the program. To
 *      do this, you can use the <code>strdup()</code> function. This function
 *      will dynamically allocate space for a string copy and will copy the
 *      passed string. Because this is a dynamic allocation, you will later need
 *      to free it (in part B). If you don't create a copy of the string,
 *      you may see repeated symbols in the program output. Type <code>man
 *      strdup</code> in the terminal to see the documentation for this
 *      function.<br /><br /></li>
 *  <li>If you don't add the new node to the beginning of the linked list,
 *      the output that your <code>symbol_iterate()</code> function generates
 *      when using the <code>list</code> command may be different from what the
 *      autograder expects and you may not get full credit.</li>
 *  </ol>
 * 
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 *  @param name - The name of the symbol.
 *  @param addr - The address of the symbol.
 */
void symbol_add_unique (sym_table_t* symTab, const char* name, int addr);

/** Search for a symbol's name given its address. This should be a simple lookup
 *  in the <code>addr_table</code>. Use the <code>label</code> command to test
 *  this function.
 *  
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 *  @param addr - An address.
 *  @return The <b>label</b> at that address or NULL if no symbol is
 *  associated with the address. 
 */
char* symbol_find_by_addr (sym_table_t* symTab, int addr);

/** Visit all the symbols in the hash table and call a function for each one.
 *  The interesting thing is that the function to be called for each node is
 *  called through a <em>function pointer</em> given to you as the
 *  <code>fnc</code> parameter. Calling a function through a function pointer is
 *  straightforward. The line of code you will use looks like this (replace the
 *  part in blue appropriately):
 * 
 *  <code>(*fnc)(<span style="color: blue; font-weight: bold;"><i>memory address
 *  of symbol</i></span>, data);</code>
 * 
 *  Why is this useful? Because whoever calls your <code>symbol_iterate()</code>
 *  function can specify what to do for each node by defining a separate
 *  function and passing you a pointer to it. It makes your program more
 *  flexible. In this assignment, we use <code>symbol_iterate()</code> for two
 *  commands: <code>list</code> and <code>count</code>. The <code>list</code>
 *  command passes a printing function to your function so that each node is
 *  shown on the screen (but you don't need to know that, you just need to call
 *  the function through the function pointer). The <code>count</code> command
 *  passes a counting function to your function (but again, you don't really
 *  need to know that). The <code>data</code> argument is passed to you as an 
 *  argument in the <code>symbol_iterate()</code> function. You don't need to do
 *  anything with this parameter. Just pass it as-is to the function that you're
 *  calling using the function pointer. Function pointers are used in
 *  event-driven programming where callback functions are needed to process
 *  events. This is a popular paradigm in computer networking.
 * 
 *  It is <span style="color: red; font-weight: bold;">very important</span>
 *  that you visit the nodes in a specific order. You must have a loop that
 *  visits each entry in the hash table starting at index 0. Every time it finds
 *  a non-NULL pointer, it must traverse the corresponding linked list. The
 *  following figure shows a hash table of size 6. The order of traversal is
 *  shown with numbers inside each node:
 * 
 *  <img summary="traversal" src="symbol_iterate_order.png">
 * 
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 *  @param fnc - Pointer to the function to be called on every symbol.
 *  @param data - Any additional information to be passed on to
 *  <code>fnc</code> directly. The called function will cast this to whatever
 *  type it expects.
 */
void symbol_iterate (sym_table_t* symTab, iterate_fnc_t fnc, void* data);

/** This function is a useful support function for the
 *  <code>symbol_add()</code> and <code>symbol_find_by_name()</code> functions.
 *  It searches for a node in the hash table whose symbol's name matches the
 *  name passed to the function. The search must be performed in a case
 *  insensitive manner.
 * 
 *  The general algorithm to search for a symbol is:
 * 
 *  <ol>
 *  <li>Calculate the hash of the symbol using the provided
 *      <code>symbol_hash()</code> function (defined in
 *      <code>symbol.c</code>).</li>
 *  <li>Based on the hash, calculate the index where the symbol should be in the
 *      hash table. Why don't we just use the hash as the index? Because the
 *      hash can be a very big number. It could exceed the bounds of our array.
 *      We have to "shrink" it down so that we get an index within bounds. To
 *      do so, we compute
 *      <code>index = hash-value % capacity-of-hash-table</code>.</li>
 *  <li>Go to that index in the hash table and search the corresponding linked
 *      list to find the symbol. To speed things up, search first for the hash
 *      value. Only when the hash values match do you need to do a
 *      <span style="color: red; font-weight: bold;">case insensitive</span>
 *      string comparison. This means, for example, that if you search for a
 *      symbol with name AB, then ab, Ab, aB, and AB are matches. It turns out
 *      that the <code>symbol_hash()</code> function is case insensitive,
 *      meaning, for example, that the hash values of ab and AB are the same.
 *      However, you must still compare strings in a case-insensitive manner
 *      (hint: do a Google search to look into the C <code>strcasecmp()</code>
 *      function).</li>
 *  <li>Return the information if the name is found, or <code>NULL</code>
 *      otherwise</li>
 *  </ol>
 * 
 *  Note that you must also return the calculated hash and index through the
 *  <code>ptrToHash</code> and <code>ptrToIndex</code> pointers (even if the
 *  node was not found). You can test your implementation by using the
 *  <code>search</code> command.
 * 
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 *  @param name - The name of the symbol.
 *  @param ptrToHash - This function must obtain the hash value of the
 *  <code>name</code> parameter and return it through this pointer (even if the
 *  node was not found).
 *  @param ptrToIndex - This function must obtain the index in which the symbol
 *  being searched for is expected to be in the hash table and must return it
 *  through this pointer (even if the node was not found).
 *  @return  A pointer to the node that contains the symbol being searched for
 *  or NULL if the symbol was not found.
 */
struct node* symbol_search (sym_table_t* symTab, const char* name, int* ptrToHash, int* ptrToIndex);

/** Add a symbol to the symbol table if there is no other symbol with the same 
 *  name already in the symbol table. Otherwise, it returns an error code. You
 *  should use the <code>symbol_search()</code> function to check if there is a
 *  name duplicate (because this check must be case insensitive). If the name is
 *  not a duplicate, you should use the <code>symbol_add_unique()</code>
 *  function to add the symbol to the table (because the insertion must be done
 *  in the hash table as well as the address table in the same manner as in the
 *  <code>symbol_add_unique()</code> function). You may test your implementation
 *  using the <code>add</code> command.
 * 
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 *  @param name - The name of the symbol.
 *  @param addr - The address of the symbol.
 *  @return 1 if the symbol is not a name duplicate and was added, 0 if the
 *  symbol is a name duplicate.
 */
int symbol_add (sym_table_t* symTab, const char* name, int addr);

/** Find a symbol by its name. The search must be case insensitive. You should
 *  use the <code>symbol_search()</code> function to do the heavy work.
 * 
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 *  @param name - The name of the symbol.
 *  @return  A pointer to the symbol or NULL if the symbol was not found.
 */
symbol_t* symbol_find_by_name (sym_table_t* symTab, const char* name);

/** Remove all the symbols from the symbol table. This involves:
 * 
 *  <ul>
 *  <li>Freeing all dynamically allocated memory associated with all nodes and
 *      strings.</li>
 *  <li>Resetting all the entries in the hash table to NULL.</li>
 *  <li>Resetting all the entries in the address table to NULL.</li>
 *  </ul>
 * 
 *  Test this function with the <code>reset</code> command. The goal is for the
 *  symbol table to be reset to the initial state so that new symbols can be
 *  added immediately after resetting. You should also use Valgrind to make sure
 *  that you don't have memory errors (like invalid reads/writes or
 *  uninitialized values). At this point, it may be too early to check for
 *  memory leaks because by the time this function returns, there will still be
 *  some blocks in the heap. You can wait until you implement
 *  <code>symbol_term()</code> to check for memory leaks. Refer to the main
 *  instructions for details on how to run Valgrind.
 * 
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 */
void symbol_reset (sym_table_t* symTab);

/** Remove all symbols from the symbol table and free all dynamically allocated
 *  memory. This function is a destructor for a symbol table. Consider using the
 *  <code>symbol_reset()</code> function and then free the remaining memory.
 *  There must not be any memory leaks. Test it with Valgrind. When you run it
 *  with the <code>\-\-leak\-check=yes</code> option, you should see the
 *  following two lines:
 * 
 *  <code>All heap blocks were freed -- no leaks are possible</code><br />
 *  <code>ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)</code>
 * 
 *  Refer to the main instructions for details on how to run Valgrind.
 * 
 *  This function is called automatically when the test driver terminates by
 *  typing the <code>quit</code> command.After executing this function, the
 *  opaque pointer to the symbol table is no longer valid.
 * 
 *  @param symTab - Pointer to a sym_table_t structure so that you can access
 *  the hash table and the address table.
 */
void symbol_term(sym_table_t* symTab);

#endif /* __SYMBOL_H__ */

