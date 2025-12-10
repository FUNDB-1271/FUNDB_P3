#!/usr/bin/expect -f
# 02
# add test after fragmentation. 
# checks for the correct usage of holes in the database file, including 
# the offset update in case of not full-usage of the hole, or deletion
# if the hole falls below a given size

set timeout -1
set programName "library"
set filename "test"
set strategy "first_fit"
# Set the base name for the control files
set control_base "test_use_deleted_books_control"

spawn rm -f $filename.db $filename.ind $filename.lst

spawn ./$programName $strategy $filename
expect "Type command and argument/s"
expect "exit"

#add first book
send "add 12346|978-2-12345680-3|El Quijote|Catedra\r"
expect "Record with BookID=12346 has been added to the database"
expect "exit"

# add second book
send  "add 12345|978-2-12345086-3|La busca y las aventuras del zorro fantastico|Catedra\r"
expect "Record with BookID=12345 has been added to the database"
expect "exit"

# add third book
send  "add 12347|978-2-12345680-4|el quijote en ingles, portugues y 23 otros idiomas|catedra y otras compañias, subsidiarias y filiales\r"
expect "Record with BookID=12347 has been added to the database"
expect "exit"

# add fourth book
send  "add 12348|978-2-12345086-3|la busca|catedra\r"
expect "Record with BookID=12348 has been added to the database"
expect "exit"

# print current index
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #46"
expect "    size: #73"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #2"
expect "    key: #12347"
expect "    offset: #127"
expect "    size: #122"
expect "Entry #3"
expect "    key: #12348"
expect "    offset: #257"
expect "    size: #36"
expect "exit"

# create first fragmentation 
send "del 12347\r"
expect "Record with BookID=12347 has been deleted"
expect "exit"

# print current holes
send "printLst\n"
expect "Entry #0"
expect     "offset: #127"
expect     "size: #122"
expect "exit"

# create second fragmentation 
send "del 12345\r"
expect "Record with BookID=12345 has been deleted"
expect "exit"

# print current fragmentations
send "printLst\n"
expect "Entry #0"
expect "    offset: #127"
expect "    size: #122"
expect "Entry #1"
expect "    offset: #46"
expect "    size: #73"
expect "exit"

# add first book to fill the internal fragmentation - since this is a first_fit program
# it will be added to the offset 127 hole. 
send "add 12343|9-978-1234512345|movie|night\r"
expect "Record with BookID=12343 has been added to the database"
expect "exit"

# print updated lst
send "printLst\n"
expect "Entry #0"
expect "    offset: #166"
expect "    size: #83"
expect "Entry #1"
expect "    offset: #46"
expect "    size: #73"
expect "exit"

# create another book that will make the first hole be smaller than the minimum 
# required size, therefore it will be deleted
send "add 12342|9-978-2345623456|el quijote pero ahora en italiano|Catedrales y monu\r"
expect "Record with BookID=12342 has been added to the database"
expect "exit"

# current DB state 
send "printInd\n"
expect "Entry #0"
expect "    key: #12342"
expect "    offset: #166"
expect "    size: #71"
expect "Entry #1"
expect "    key: #12343"
expect "    offset: #127"
expect "    size: #31"
expect "Entry #2"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #3"
expect "    key: #12348"
expect "    offset: #257"
expect "    size: #36"
expect "exit"

# current lst struct, only one element
send "printLst\n"
expect "Entry #0"
expect "    offset: #46"
expect "    size: #73"
expect "exit"

send "exit\n"
expect "all done"

puts "1) Use deleted books OK, ;-)"

# --- Start of Hard-Coded File Comparisons ---

# Compare .db file
puts "3) Comparing $filename.db with ${control_base}.db..."
set output_db "differ"
try {
    set output_db [exec diff -s $filename.db ${control_base}.db]
} trap CHILDSTATUS {} {}

if {[regexp -nocase "identical" $output_db] || [regexp -nocase "idénticos" $output_db]} {
    puts "   Files $filename.db are identical, ;-)"
} else {
    puts "   Files $filename.db differ, :-("
}

# Compare .ind file
puts "3) Comparing $filename.ind with ${control_base}.ind..."
set output_ind "differ"
try {
    set output_ind [exec diff -s $filename.ind ${control_base}.ind]
} trap CHILDSTATUS {} {}

if {[regexp -nocase "identical" $output_ind] || [regexp -nocase "idénticos" $output_ind]} {
    puts "   Files $filename.ind are identical, ;-)"
} else {
    puts "   Files $filename.ind differ, :-("
}

# Compare .lst file
puts "3) Comparing $filename.lst with ${control_base}.lst..."
set output_lst "differ"
try {
    set output_lst [exec diff -s $filename.lst ${control_base}.lst]
} trap CHILDSTATUS {} {}

if {[regexp -nocase "identical" $output_lst] || [regexp -nocase "idénticos" $output_lst]} {
    puts "   Files $filename.lst are identical, ;-)"
} else {
    puts "   Files $filename.lst differ, :-("
}

# --- End of Hard-Coded File Comparisons ---

puts "4) Script end"