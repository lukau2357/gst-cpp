- Refaktor jednog dana...

- all ocurrences of the given query in all input strings. For ordinary suffix tree this takes O(m + z) where m is the length of the query and z is the number of occurrences of the query in the input string. Same behvaiour is exhibited by GST.

- longest pattern that appears in all input strings - linear in cumulative length of all input strings. Possibly optimize the present implementation?

- all common substrings of given minimum length - also linear in cumulative length of all input strings.

- All GST queries are resolved through DFS traversals of the GST with query-specific tweaks.

- Eventualno uvesti da svi stringovi dijele isti terminacioni karakter, to ce da dovede do promjena u update proceduri, moglo bi cak i da dovede do asimptotski losijeg algoritma?
