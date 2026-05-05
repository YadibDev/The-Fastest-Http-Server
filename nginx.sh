find . -name "*.cpp" | while read -r file; do
  awk '
    BEGIN { in_comment=0 }
    {
      line = $0
      if (in_comment) {
        comment = comment "\n" line
        if (match(line, /\*\//)) {
          in_comment=0
          comment_end=RSTART+RLENGTH-1
          comment_text = substr(comment, 1, comment_end)
          print FILENAME ":" comment_start_line ":/*" comment_text "*/"
          comment=""
        }
      } else if (match(line, /\/\*/)) {
        in_comment=1
        comment_start_line=NR
        comment = substr(line, RSTART)
        if (match(line, /\*\//, RSTART)) {
          in_comment=0
          comment_end=RSTART+RLENGTH-1
          comment_text = substr(comment, 1, comment_end)
          print FILENAME ":" comment_start_line ":/*" comment_text "*/"
          comment=""
        }
      } else if (match(line, /\/\//)) {
        print FILENAME ":" NR "://" substr(line, RSTART+2)
      }
    }
  ' "$file"
done