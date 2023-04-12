- Refaktor jednog dana...

- all ocurrences of the given query in all input strings. Complexity analysis:
    1. O(m + z) for ordinary suffix tree, preprocessing is O(n)
    2. O(m + z) for GST, preprocessing is $O(\sum_{i=1}^{k}n_{i})$. This also covers all strings that contain the 
       given query (bitmask representation, number of set bits...). Here z is the number of occurences of the query
       accross all input strings.
    3. If we want to get all occurences, extra work needs to be done. For each leaf, we keep a map from stringIndex
       to a vector of positions in which the given pattern occurs. In this case, the complexity would be
       $O(m + kz)$ where $z$ is the number of occurences?

- k-common substring problem - linear in cumulative length of all input strings, assuming the operation "check if a set     contains k elements" and "add element to a set" takes constant time. Further complicated if we are interested in occurences
of k-common substrings in all input strings.

- Special function for only solving the longest common substring problem, done in linear time, avoids complex string to
  ocurrence mappings introduced by previous two functions since we are not

- All GST queries are resolved through DFS traversals of the GST with query-specific tweaks.

- Eventualno uvesti da svi stringovi dijele isti terminacioni karakter, to ce da dovede do promjena u update proceduri, moglo bi cak i da dovede do asimptotski losijeg algoritma?