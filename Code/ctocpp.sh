for files in *.c
do
 mv "$files" "${files%.c}.cpp"
done
