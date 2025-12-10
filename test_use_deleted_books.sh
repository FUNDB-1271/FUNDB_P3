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
set control_base "${filename}_use_deleted_books_control"
# List of extensions to compare
set extensions [list ".db" ".ind" ".lst"]

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
send "del 12347"
expect "Record with BookID=12347 has been deleted"
expect "exit"

# print current holes
send "printLst"
expect "Entry #0"
expect     "offset: #127"
expect     "size: #122"
expect "exit"

# create second fragmentation 
send "del 12345"
expect "Record with BookID=12345 has been deleted"
expect "exit"

# print current fragmentations
send "printLst"
expect "Entry #0"
expect "    offset: #127"
expect "    size: #122"
expect "Entry #1"
expect "    offset: #46"
expect "    size: #73"
expect "exit"

# add first book to fill the internal fragmentation - since this is a first_fit program
# it will be added to the offset 127 hole. 
send "add 12343|9-978-1234512345|movie|night"
expect "Record with BookID=12343 has been added to the database"
expect "exit"

# print updated lst
send "printLst"
expect "Entry #0"
expect "    offset: #166"
expect "    size: #83"
expect "Entry #1"
expect "    offset: #46"
expect "    size: #73"
expect "exit"

# create another book that will make the first hole be smaller than the minimum 
# required size, therefore it will be deleted
send "add 12342|9-978-2345623456|el quijote pero ahora en italiano|Catedrales y monu"
expect "Record with BookID=12342 has been added to the database"
expect "exit"

# current DB state 
send "printInd"
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
send "printLst"
expect "Entry #0"
expect "    offset: #46"
expect "    size: #73"
expect "exit"

puts "1) Use deleted books OK, ;-)"

set comparison_status "identical"

# Loop through the desired file extensions and compare
foreach ext $extensions {
    set test_file "$filename$ext"
    set control_file "$control_base$ext"
    
    puts "3) Comparing $test_file with $control_file..."
    
    set output "differ"
    
    # Execute diff command and check the exit status using 'catch'
    if {[catch {exec diff -s $test_file $control_file} output]} {
        # diff returns non-zero status (1) if files differ
        if {[regexp -nocase "(No such file or directory|cannot stat|No se encontró el archivo)" $output]} {
            puts "   Comparison failed: $test_file or $control_file not found. :-("
        } else {
            # Files exist but differ
            puts "   Files differ, :-("
        }
        set comparison_status "differ"
    } else {
        # diff returns zero status (0) if files are identical
        if {[regexp -nocase "identical" $output] || [regexp -nocase "idénticos" $output]} {
            puts "   Files are identical, ;-)"
        } else {
            # A safeguard for unexpected output when exit status is 0
            puts "   Files differ (unexpected diff output), :-("
            set comparison_status "differ"
        }
    }
}

# Final summary based on all comparisons
if {[string equal $comparison_status "identical"]} {
    puts "3) All files (.db, .ind, .lst) are identical, ;-)"
} else {
    puts "3) At least one file differs, :-("
}

puts "4) Script end"